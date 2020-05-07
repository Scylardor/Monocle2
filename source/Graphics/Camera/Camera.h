// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Math/Matrix.h"

#include "ViewportHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{
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


	/**
	 * \brief A graphics APi-agnostic abstract class for a camera object.
	 */
	class ACamera
	{
	public:
		ACamera() = default;

		Monocle_Graphics_API ACamera(ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc);
		Monocle_Graphics_API ACamera(ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc);


		void	SetOrtographic(const OrthographicCameraDesc& orthoDesc);
		void	SetPerspective(const PerspectiveCameraDesc& perspecDesc);

		void	SetFoVY(Degs_f newFovY);
		void	SetAspectRatio(float newAspect);

		void	SetNear(float zNear);
		void	SetFar(float zFar);


		[[nodiscard]] const Mat4&	GetViewMatrix() const { return m_viewMatrix; }

		[[nodiscard]] const Mat4&	GetProjectionMatrix() const { return m_projectionMatrix; }

		[[nodiscard]] const Mat4&	GetViewProjectionMatrix() const { return m_modelViewMatrix; }

		[[nodiscard]] ViewportHandle	GetViewportHandle() const { return m_viewportHandle; }


	protected:

		void	ComputeProjectionMatrix();

	private:

		CameraProjection	m_projectionType{CameraProjection::Perspective};

		CameraData	m_cameraData;

		ViewportHandle	m_viewportHandle{ViewportHandle::Null()};

		Mat4	m_viewMatrix{Mat4::Identity()};
		Mat4	m_projectionMatrix{ Mat4::Identity() };
		Mat4	m_modelViewMatrix{ Mat4::Identity() };
	};


}