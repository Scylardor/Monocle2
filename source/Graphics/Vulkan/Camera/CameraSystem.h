#pragma once
#include "Graphics/Vulkan/DescriptorSet/VulkanDescriptorSetAllocator.h"
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"
#include "Graphics/Camera/Camera.h"
#include "Core/Containers/SparseArray/SparseArray.h"

#include "Math/Matrix.h"
#include "Math/Vec4.h"

#include <variant>


namespace moe
{
	class VulkanCameraSystem;
	class MyVkDevice;


	// 255 cameras at most should be enough
	using CameraID = uint8_t;
	inline static const CameraID INVALID_CAMERA = 0xFF;

	/* Camera structure sent to the shaders. */
	struct GPUCamera
	{
		Mat4	m_view;
		Mat4	m_proj;
		Mat4	m_viewProj;
		Vec4	m_cameraPos{ 0 };
	};


	struct ViewportDesc
	{
		ViewportDesc(float px, float py, float pw, float ph, float mind, float maxd) :
			x(px), y(py), Width(pw), Height(ph), MinDepth(mind), MaxDepth(maxd)
		{}

		float	x{0}, y{0};
		float	Width{ 0 }, Height{ 0 };
		float	MinDepth{ 0 }, MaxDepth{ 1 };
	};

#define MOE_FULLSCREEN_VIEWPORT	ViewportDesc(0, 0, 1.f, 1.f, 0, 1)


	struct ScissorDesc
	{
		ScissorDesc(int32_t px, int32_t py, uint32_t pw, uint32_t ph) :
			x(px), y(py), Width(pw), Height(ph)
		{}

		int32_t		x{ 0 }, y{ 0 };
		uint32_t	Width{ 0 }, Height{ 0 };
	};

#define MOE_FULLSCREEN_SCISSOR	ScissorDesc(0, 0, 1, 1)


	// A camera cannot be orthographic AND perspective at the same time, so let's share the data.
	struct CameraProjectionVar
	{
		CameraProjectionVar() = default;

		CameraProjectionVar(const OrthographicCameraDesc& orthoData) :
			m_projection(orthoData)
		{}

		CameraProjectionVar(const PerspectiveCameraDesc& persData) :
			m_projection(persData)
		{}

		std::variant<OrthographicCameraDesc, PerspectiveCameraDesc>	m_projection;
	};

	/* Camera description we keep on the CPU. */
	struct CameraDesc
	{
		CameraDesc(const OrthographicCameraDesc& ortho, const Vec3& pos) :
			WorldPosition(pos),
			ProjectionData(ortho)
		{}

		CameraDesc(const PerspectiveCameraDesc& persp, const Vec3& pos) :
			WorldPosition(pos),
			ProjectionData(persp)
		{}

		Vec3				WorldPosition{ Vec3::ZeroVector() };
		float				MoveSpeed{ 2.5f };
		Degs_f				RotationYaw{ -90.f }; // Point towards the negative Z-axis by default
		Degs_f				RotationPitch{ 0.f };
		Vec3				RightVector{ 1.f, 0.f, 0.f };
		Vec3				UpVector{ 0.f, 1.f, 0.f };
		Vec3				FrontVector{ 0.f, 0.f, 1.f };
		//std::optional<Vec3>	LookatTarget{};
		CameraProjectionVar	ProjectionData;
		CameraID			ID{INVALID_CAMERA};
		bool				UpdatedSinceLastRender{ false };
	};


	struct CameraRef
	{
	public:
		CameraRef() = default;

		CameraRef(VulkanCameraSystem& system, uint8_t id) :
			m_system(&system), m_camID(id)
		{}

		CameraRef(CameraRef&& other) noexcept
		{
			*this = std::move(other);
		}


		CameraRef& operator=(CameraRef&& other) noexcept
		{
			if (this != &other)
			{
				m_system = other.m_system;
				m_camID = other.m_camID;

				other.m_camID = INVALID_CAMERA;
			}
			return *this;
		}



		CameraRef& Lookat(const Vec3& worldTarget);


		CameraRef&			SetPosition(const Vec3& worldPos);
		[[nodiscard]] Vec3	GetPosition() const;


		[[nodiscard]] const Vec3&	GetUpVector() const;

		[[nodiscard]] const Vec3&	GetRightVector() const;

		[[nodiscard]] const Vec3&	GetFrontVector() const;

		CameraRef&			SetNearPlane(float near);
		[[nodiscard]] float	GetNearPlane() const;

		CameraRef&			SetFarPlane(float far);
		[[nodiscard]] float	GetFarPlane() const;

		// Orthographic only
		CameraRef&			SetLeftPlane(float left);
		[[nodiscard]] float	GetLeftPlane() const;

		CameraRef&			SetRightPlane(float right);
		[[nodiscard]] float	GetRightPlane() const;

		// Perspective only
		CameraRef&				SetFieldOfViewY(Degs_f fovy);
		[[nodiscard]] Degs_f	GetFieldOfViewY() const;

		CameraRef&			SetAspectRatio(float ar);
		[[nodiscard]] float	GetAspectRatio() const;

		CameraRef&	SetViewport(const ViewportDesc& desc);
		CameraRef&	SetScissor(const ScissorDesc& desc);

		[[nodiscard]] const Mat4& GetViewProjection() const;

		void	MoveForward(float dt);
		void	MoveBackward(float dt);
		void	StrafeLeft(float dt);
		void	StrafeRight(float dt);

		void	Rotate(float yawOffset, float pitchOffset);


	private:

		VulkanCameraSystem* m_system{nullptr};
		CameraID			m_camID{ INVALID_CAMERA };
	};


	/* Resource handler for everything related to GPU viewport information (Vulkan version) */
	class VulkanCameraSystem
	{
		static const auto DEFAULT_CAMERA_SET_NUMBER = 5u;
		static const auto DEFAULT_NUMBER_OF_CAMERAS = 4u;  // 4 is enough for now

	public:


		VulkanCameraSystem();


		~VulkanCameraSystem();


		void	Initialize(MyVkDevice& device, const VulkanSwapchain& swapChain, uint32_t frameCount);

		void	Update(uint32_t frameIndex);


		template <typename TCam, typename... Args>
		CameraRef	New(const ViewportDesc& vpDesc, const ScissorDesc& scDesc, const Vec3& worldPos, Args&&... args)
		{
			TCam camDesc{ std::forward<Args>(args)... };
			auto camID = m_cameras.Emplace(std::move(camDesc), worldPos);
			CameraDesc& cam = m_cameras.Mut(camID);
			cam.ID = camID;

			InitializeViewportScissor(camID, vpDesc, scDesc);

			AllocateCameraDescriptors(camID);

			RecomputeProjectionMatrices(camID);

			return { *this, camID };
		}

		[[nodiscard]] const CameraDesc&	GetCamera(CameraID camID) const
		{
			return m_cameras.Get(camID);
		}



		void Lookat(CameraID camID, const Vec3& worldTarget);


		void				SetPosition(CameraID camID, const Vec3& worldPos);
		[[nodiscard]] Vec3	GetPosition(CameraID camID) const;


		[[nodiscard]] const Vec3& GetUpVector(CameraID camID) const;

		[[nodiscard]] const Vec3& GetFrontVector(CameraID camID) const;

		[[nodiscard]] const Vec3& GetRightVector(CameraID camID) const;


		void SetNearPlane(CameraID camID, float near);
		[[nodiscard]] float	GetNearPlane(CameraID camID) const;

		void SetFarPlane(CameraID camID, float far);
		[[nodiscard]] float	GetFarPlane(CameraID camID) const;

		// Orthographic only
		void				SetLeftPlane(CameraID camID, float left);
		[[nodiscard]] float	GetLeftPlane(CameraID camID) const;

		void				SetRightPlane(CameraID camID, float right);
		[[nodiscard]] float	GetRightPlane(CameraID camID) const;

		void				SetTopPlane(CameraID camID, float top);
		[[nodiscard]] float	GetTopPlane(CameraID camID) const;

		void				SetBottomPlane(CameraID camID, float bottom);
		[[nodiscard]] float	GetBottomPlane(CameraID camID) const;


		// Perspective only
		void					SetFieldOfViewY(CameraID camID, Degs_f fovy);
		[[nodiscard]] Degs_f	GetFieldOfViewY(CameraID camID) const;


		void				SetAspectRatio(CameraID camID, float ar);
		[[nodiscard]] float	GetAspectRatio(CameraID camID) const;


		void	SetViewport(CameraID camID, const ViewportDesc& desc);
		void	SetScissor(CameraID camID, const ScissorDesc& desc);

		[[nodiscard]] const Mat4& GetViewProjection(CameraID camID) const;

		[[nodiscard]] const std::pair<vk::Viewport, vk::Rect2D>& GetCameraViewportScissor(CameraID camID) const
		{
			return m_cameraViewportScissor[camID];
		}


		void	MoveForward(CameraID camID, float dt);
		void	MoveBackward(CameraID camID, float dt);
		void	StrafeLeft(CameraID camID, float dt);
		void	StrafeRight(CameraID camID, float dt);

		void	Rotate(CameraID camID, float yawOffset, float pitchOffset);


		template <typename TFunctor>
		void	ForeachCamera(TFunctor&& visitorFn)
		{
			m_cameras.ForEach([&visitorFn](CameraDesc& camDesc)
				{
					visitorFn(camDesc);
				});
		}


	protected:



	private:

		void	InitializeGPUResources(MyVkDevice& device);

		void	InitializeCameraUpdateInfos(MyVkDevice& device);

		void	AllocateCameraDescriptors(CameraID camID);

		void	InitializeViewportScissor(CameraID camId, const ViewportDesc& vpDesc, const ScissorDesc& scDesc);

		void	ResetGPUUploadCounter()
		{
			m_uploadCounter = m_frameCount;
		}


		void	RecomputeViewVectors(CameraDesc& cam);
		void	RecomputeViewMatrices(CameraDesc& cam);
		void	RecomputeProjectionMatrices(CameraID camID);

		void	RecomputeOrthographicMatrix(CameraDesc& cam, const OrthographicCamera& orthoData);
		void	RecomputePerspectiveMatrix(CameraDesc& cam, const PerspectiveCameraDesc& perspectiveData);



		MOE_VK_DEVICE_GETTER()

		MyVkDevice*				m_device{ nullptr };
		const VulkanSwapchain*	m_swapChain{ nullptr };

		SparseArray< CameraDesc, CameraID >	m_cameras{};

		VulkanBuffer			m_cameraBuffer{};
		GPUCamera*				m_camerasDeviceMemory{ nullptr };	// The cameras sent on the GPU.
		std::vector<GPUCamera>	m_camerasHostMemory;				// The cameras data read and written on the CPU.

		uint32_t		m_uploadCounter{ 0 };	// Number of frames we should consider the GPU data 'dirty'.

		uint32_t		m_frameCount{ 0 }; // Maximum number of frames in flight

		VulkanDescriptorSetAllocator	m_setAllocator{};

		std::vector<vk::DescriptorSet>	m_cameraDescriptors{};

		std::vector<std::pair<vk::Viewport, vk::Rect2D>>	m_cameraViewportScissor{};
	};


}


#endif // MOE_VULKAN
