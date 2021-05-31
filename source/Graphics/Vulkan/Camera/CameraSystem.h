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
			WorldTransform(Mat4::Translation(pos)),
			ProjectionData(ortho)
		{}

		CameraDesc(const PerspectiveCameraDesc& persp, const Vec3& pos) :
			WorldTransform(Mat4::Translation(pos)),
			ProjectionData(persp)
		{}

		Mat4				WorldTransform{ Mat4::Identity() };
		Vec3				UpVector{ 0, 1, 0 };
		Vec3				RightVector{ 1, 0, 0 };
		CameraProjectionVar	ProjectionData;
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


		CameraRef&					SetUpVector(const Vec3& upVec);
		[[nodiscard]] const Vec3&	GetUpVector() const;

		CameraRef&					SetRightVector(const Vec3& rightVec);
		[[nodiscard]] const Vec3&	GetRightVector() const;


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


		[[nodiscard]] const Mat4& GetViewProjection() const;

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


		void	Initialize(MyVkDevice& device, uint32_t frameCount);

		void	Update(uint32_t frameIndex);


		template <typename TCam, typename... Args>
		CameraRef	New(const Vec3& worldPos, Args&&... args)
		{
			TCam camDesc{ std::forward<Args>(args)... };
			auto camID = m_cameras.Emplace(camDesc, worldPos);

			RecomputeViewProjection(camID);
			return { *this, camID };
		}

		[[nodiscard]] const CameraDesc&	GetCamera(CameraID camID) const
		{
			return m_cameras.Get(camID);
		}



		void Lookat(CameraID camID, const Vec3& worldTarget);


		void SetPosition(CameraID camID, const Vec3& worldPos);
		[[nodiscard]] Vec3	GetPosition(CameraID camID) const;


		void SetUpVector(CameraID camID, const Vec3& upVec);
		[[nodiscard]] const Vec3& GetUpVector(CameraID camID) const;

		void SetRightVector(CameraID camID, const Vec3& rightVec);
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

		// TODO: Top and Bottom are missing!
		void				SetTopPlane(CameraID camID, float top);
		[[nodiscard]] float	GetTopPlane(CameraID camID) const;

		void				SetBottomPlane(CameraID camID, float bottom);
		[[nodiscard]] float	GetBottomPlane(CameraID camID) const;


		// Perspective only
		void SetFieldOfViewY(CameraID camID, Degs_f fovy);
		[[nodiscard]] Degs_f	GetFieldOfViewY(CameraID camID) const;


		void				SetAspectRatio(CameraID camID, float ar);
		[[nodiscard]] float	GetAspectRatio(CameraID camID) const;


		[[nodiscard]] const Mat4& GetViewProjection(CameraID camID) const;


	protected:



	private:

		void	SetDirty()
		{
			m_dirtyCounter = m_frameCount;
		}


		void	RecomputeViewProjection(CameraID camID);

		void		RecomputeOrthographicMatrix(CameraID camID, const OrthographicCamera& orthoData);
		static void	RecomputePerspectiveMatrix(GPUCamera& cameraMatrices, const PerspectiveCameraDesc& perspectiveData);


		void	UpdateGPUCameras();

		MOE_VK_DEVICE_GETTER()

		MyVkDevice*		m_device{ nullptr };

		SparseArray< CameraDesc, CameraID >	m_cameras{};

		VulkanBuffer	m_cameraBuffer{};
		GPUCamera*				m_camerasDeviceMemory{ nullptr };	// The cameras sent on the GPU.
		std::vector<GPUCamera>	m_camerasHostMemory;				// The cameras data read and written on the CPU.

		std::vector<std::pair<CameraID, uint8_t>>	m_dirtyCounters{};
		uint32_t		m_dirtyCounter{ 0 };	// Number of frames we should consider the GPU data 'dirty'.

		uint32_t		m_frameCount{ 0 }; // Maximum number of frames in flight

		VulkanDescriptorSetAllocator	m_setAllocator{};

	};
}


#endif // MOE_VULKAN
