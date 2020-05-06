// Monocle Game Engine source files - Alexandre Baron

#include "Camera.h"

namespace moe
{
	ACamera::ACamera(ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc) :
		m_projectionType{CameraProjection::Orthographic},
		m_cameraData(orthoDesc),
		m_viewportHandle(vpHandle)
	{
		ComputeProjectionMatrix();
	}

	ACamera::ACamera(ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc) :
		m_projectionType{CameraProjection::Perspective},
		m_cameraData(perspecDesc),
		m_viewportHandle(vpHandle)
	{
		ComputeProjectionMatrix();
	}


	void ACamera::SetFoVY(Degs_f newFovY)
	{
		m_projectionType = CameraProjection::Perspective;
		m_cameraData.m_perspective.m_fovY = newFovY;
		// Fov has changed - recompute proj matrix
		ComputeProjectionMatrix();
	}


	void ACamera::SetAspectRatio(float newAspect)
	{
		m_projectionType = CameraProjection::Perspective;
		m_cameraData.m_perspective.m_aspectRatio = newAspect;
		// Aspect has changed - recompute proj matrix
		ComputeProjectionMatrix();
	}


	void ACamera::SetNear(float zNear)
	{
		switch (m_projectionType)
		{
		case CameraProjection::Orthographic:
			m_cameraData.m_ortho.m_near = zNear;
			break;
		case CameraProjection::Perspective:
			m_cameraData.m_perspective.m_near = zNear;
			break;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unsupported camera projection type");
		}

		// Near has changed - recompute proj matrix
		ComputeProjectionMatrix();
	}


	void ACamera::SetFar(float zFar)
	{
		switch (m_projectionType)
		{
		case CameraProjection::Orthographic:
			m_cameraData.m_ortho.m_far = zFar;
			break;
		case CameraProjection::Perspective:
			m_cameraData.m_perspective.m_far = zFar;
			break;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unsupported camera projection type");
		}

		// Far has changed - recompute proj matrix
		ComputeProjectionMatrix();
	}


	void ACamera::ComputeProjectionMatrix()
	{
		switch (m_projectionType)
		{
		case CameraProjection::Orthographic:
			m_projectionMatrix = Mat4::Orthographic(
				m_cameraData.m_ortho.m_left, m_cameraData.m_ortho.m_right,
				m_cameraData.m_ortho.m_bottom, m_cameraData.m_ortho.m_top,
				m_cameraData.m_ortho.m_near, m_cameraData.m_ortho.m_far);
			break;
		case CameraProjection::Perspective:
			m_projectionMatrix = Mat4::Perspective(
				Rads_f(m_cameraData.m_perspective.m_fovY), m_cameraData.m_perspective.m_aspectRatio,
				m_cameraData.m_perspective.m_near, m_cameraData.m_perspective.m_far);
			break;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unsupported camera projection type");
		}
	}
}
