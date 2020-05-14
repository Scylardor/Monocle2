// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Math/Matrix.h"

#include "ViewportHandle.h"

#include "Graphics/RenderWorld/GraphicsObject.h"

#include "Graphics/DeviceBuffer/UniformBufferHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{
	class RenderWorld;

	enum class CameraProjection : uint8_t
	{
		Orthographic,
		Perspective
	};

	struct CameraDescriptor
	{
		CameraProjection	m_proj{CameraProjection::Perspective};

	};

	struct OrthographicCameraDesc
	{
		float	m_left = 0.0f;
		float	m_right = 800.0f;
		float	m_bottom = 0.0f;
		float	m_top = 600.0f;
		float	m_near = 0.1f;
		float	m_far = 100.0f;
	};


	struct PerspectiveCameraDesc
	{
		Degs_f				m_fovY{ 90.F };	// The perspective vertical field of view (Y), in degrees.
		float				m_aspectRatio{ 1.f };	// The aspect ratio (width / height).
		float				m_near = { 0.1f };
		float				m_far = { 1000.f };
	};

	// A camera cannot be orthographic AND perspective at the same time, so let's share the data.
	union CameraData
	{
		CameraData(const CameraData& other) :
			m_ortho(other.m_ortho)
		{}

		CameraData(const OrthographicCameraDesc& orthoData) :
			m_ortho(orthoData)
		{}

		CameraData(const PerspectiveCameraDesc& persData) :
			m_perspective(persData)
		{}

		OrthographicCameraDesc	m_ortho;
		PerspectiveCameraDesc	m_perspective;
	};


	struct CameraMatrices
	{
		CameraMatrices(const Mat4& view = Mat4::Identity(), const Mat4 & proj = Mat4::Identity(), const Mat4& viewProj = Mat4::Identity()) :
			m_view(view), m_proj(proj), m_viewProj(viewProj)
		{}

		Mat4	m_view;
		Mat4	m_proj;
		Mat4	m_viewProj;
	};


	/**
	 * \brief A graphics APi-agnostic abstract class for a camera object.
	 */
	class Camera : public AGraphicObject
	{
	public:
		Camera() = default;

		Monocle_Graphics_API Camera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc, const Transform& transf = Transform::Identity());
		Monocle_Graphics_API Camera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc, const Transform& transf = Transform::Identity());


		void	SetOrthographic(const OrthographicCameraDesc& orthoDesc);
		void	SetPerspective(const PerspectiveCameraDesc& perspecDesc);

		Monocle_Graphics_API [[nodiscard]] Degs_f	GetFovY() const;
		Monocle_Graphics_API void					SetFoVY(Degs_f newFovY);
		Monocle_Graphics_API void					AddFoVY(Degs_f addedFovY) { SetFoVY(Degs_f(m_cameraData.m_perspective.m_fovY + addedFovY)); }

		void	SetAspectRatio(float newAspect);

		void	SetNear(float zNear);
		void	SetFar(float zFar);


		[[nodiscard]] const Mat4&	GetViewMatrix() const { return m_matrices.m_view; }

		[[nodiscard]] const Mat4&	GetProjectionMatrix() const { return m_matrices.m_proj; }

		[[nodiscard]] const Mat4&	GetViewProjectionMatrix() const { return m_matrices.m_viewProj; }

		[[nodiscard]] ViewportHandle	GetViewportHandle() const { return m_viewportHandle; }


		Monocle_Graphics_API void SetTransform(const Transform& transf) override final;
		Monocle_Graphics_API const Transform& AddTransform(const Transform& transf) override final;


		/**
		 * \brief Sets the camera transform to the lookat matrix described by provided vectors.
		 * This version doesn't change position : will use the current transform's one.
		 * \param lookatPointWorld The point to look at in world space
		 * \param upWorld The reference up vector in world space
		 * \return The new camera transform
		 */
		Monocle_Graphics_API const Transform& LookAt(const Vec3& lookatPointWorld, const Vec3& upWorld);


		/**
		 * \brief Sets the camera transform to the lookat matrix described by provided vectors.
		 * \param cameraPosWorld New camera position in world space
		 * \param lookatPointWorld The point to look at in world space
		 * \param upWorld The reference up vector in world space
		 * \return The new camera transform
		 */
		Monocle_Graphics_API const Transform& LookAt(const Vec3& cameraPosWorld, const Vec3& lookatPointWorld, const Vec3& upWorld);


		const CameraMatrices&	GetCameraMatrices() const
		{
			return m_matrices;
		}


		DeviceBufferHandle	GetCameraGpuHandle() const
		{
			return m_graphicData.m_uniformDataHandle;
		}


		[[nodiscard]] const Vec3&	GetFrontVector() const	{ return m_cameraFront; }
		void	SetFrontVector(const Vec3& newFront) { m_cameraFront = newFront; }

		[[nodiscard]] const Vec3&	GetUpVector() const		{ return m_cameraUp; }
		void	SetUpVector(const Vec3& newUp) { m_cameraUp = newUp; }

		[[nodiscard]] const Vec3&	GetRightVector() const { return m_cameraRight; }
		void	SetRightVector(const Vec3& newRight) { m_cameraRight = newRight; }

		float	GetYaw() const { return m_yaw; }
		float	GetPitch() const { return m_pitch; }

		Monocle_Graphics_API void	UpdateCameraVectors(float pitch, float yaw);

	protected:

		/* TODO: code smell: camera matrices meant for GPU usage should not be recomputed on each transform update... */
		void	RecomputeViewMatrices();

		void	ComputeProjectionMatrices();

	private:
		CameraMatrices	m_matrices;

		CameraData	m_cameraData;

		ViewportHandle	m_viewportHandle{0};

		CameraProjection	m_projectionType{ CameraProjection::Perspective };

		Vec3	m_cameraFront{0, 0, -1};
		Vec3	m_cameraRight{ 1, 0, 0 };
		Vec3	m_cameraUp{ 0, 1, 0 };
		Degs_f	m_yaw{0.f};
		Degs_f	m_pitch{0.f};
		Degs_f	m_pitchThreshold{89.f};

		/* Threshold values for Perspective camera type */
		Degs_f	m_minFoVY{1.f};
		Degs_f	m_maxFoVY{45.f};
	};


}