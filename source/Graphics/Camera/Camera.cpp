// Monocle Game Engine source files - Alexandre Baron

#include "Camera.h"

namespace moe
{
	Camera::Camera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc, const Transform& transf) :
		AGraphicObject(world, data, transf),
		m_cameraData(orthoDesc),
		m_viewportHandle(vpHandle),
		m_projectionType {CameraProjection::Orthographic}
	{
		ComputeProjectionMatrix();
	}

	Camera::Camera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc, const Transform& transf) :
		AGraphicObject(world, data, transf),
		m_cameraData(perspecDesc),
		m_viewportHandle(vpHandle),
		m_projectionType{ CameraProjection::Perspective }
	{
		ComputeProjectionMatrix();
	}


	void Camera::SetFoVY(Degs_f newFovY)
	{
		m_projectionType = CameraProjection::Perspective;
		m_cameraData.m_perspective.m_fovY = newFovY;
		// Fov has changed - recompute proj matrix
		ComputeProjectionMatrix();
	}


	void Camera::SetAspectRatio(float newAspect)
	{
		m_projectionType = CameraProjection::Perspective;
		m_cameraData.m_perspective.m_aspectRatio = newAspect;
		// Aspect has changed - recompute proj matrix
		ComputeProjectionMatrix();
	}


	void Camera::SetNear(float zNear)
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


	void Camera::SetFar(float zFar)
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


	void Camera::SetTransform(const Transform& transf)
	{
		AGraphicObject::SetTransform(transf);

		// When transformation changes, recompute view and view projection matrices, as they are dependent on the camera transform.
		RecomputeViewMatrices();
	}


	const Transform& Camera::AddTransform(const Transform& transf)
	{
		const Transform& newTransf = AGraphicObject::AddTransform(transf);

		RecomputeViewMatrices();

		return newTransf;
	}


	void Camera::RecomputeViewMatrices()
	{
		m_matrices.m_view = m_transform.Matrix().GetInverse();
		m_matrices.m_viewProj = m_matrices.m_proj * m_matrices.m_view;

		m_transformIsUpToDate = false;
	}


	void Camera::ComputeProjectionMatrix()
	{
		switch (m_projectionType)
		{
		case CameraProjection::Orthographic:
			m_matrices.m_proj = Mat4::Orthographic(
				m_cameraData.m_ortho.m_left, m_cameraData.m_ortho.m_right,
				m_cameraData.m_ortho.m_bottom, m_cameraData.m_ortho.m_top,
				m_cameraData.m_ortho.m_near, m_cameraData.m_ortho.m_far);
			break;
		case CameraProjection::Perspective:
			m_matrices.m_proj = Mat4::Perspective(
				Rads_f(m_cameraData.m_perspective.m_fovY), m_cameraData.m_perspective.m_aspectRatio,
				m_cameraData.m_perspective.m_near, m_cameraData.m_perspective.m_far);
			break;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unsupported camera projection type");
		}
	}
}
