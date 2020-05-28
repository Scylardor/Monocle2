// Monocle Game Engine source files - Alexandre Baron

#include "Camera.h"

#include "CameraSystem.h"

namespace moe
{
	Camera::Camera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc, const Transform& transf) :
		AGraphicObject(world, data, transf),
		m_cameraData(orthoDesc),
		m_viewportHandle(vpHandle),
		m_projectionType {CameraProjection::Orthographic}
	{
		ComputeProjectionMatrices();
	}

	Camera::Camera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc, const Transform& transf) :
		AGraphicObject(world, data, transf),
		m_cameraData(perspecDesc),
		m_viewportHandle(vpHandle),
		m_projectionType{ CameraProjection::Perspective }
	{
		ComputeProjectionMatrices();
	}


	Camera::Camera(CameraSystem* parentSystem, ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc, CameraMatrices* matricesMem) :
		AGraphicObject(nullptr, GraphicObjectData()),
		m_parentSystem(parentSystem),
		m_cameraData(perspecDesc),
		m_viewportHandle(vpHandle),
		m_projectionType{ CameraProjection::Perspective },
		m_matricesDataPtr(matricesMem)

	{
		ComputeProjectionMatrices();
	}


	Camera::Camera(CameraSystem* parentSystem, ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc, CameraMatrices* matricesMem) :
		AGraphicObject(nullptr, GraphicObjectData()),
		m_parentSystem(parentSystem),
		m_cameraData(orthoDesc),
		m_viewportHandle(vpHandle),
		m_projectionType{ CameraProjection::Orthographic },
		m_matricesDataPtr(matricesMem)
	{
		ComputeProjectionMatrices();
	}


	Camera::Camera(CameraSystem* parentSystem, ViewportHandle vpHandle, const CameraData& camData, CameraProjection projType, CameraMatrices* matricesMem) :
		AGraphicObject(nullptr, GraphicObjectData()),
		m_parentSystem(parentSystem),
		m_cameraData(camData),
		m_viewportHandle(vpHandle),
		m_projectionType{ projType },
		m_matricesDataPtr(matricesMem)
	{
		ComputeProjectionMatrices();
	}


	Degs_f Camera::GetFovY() const
	{
		// It makes no sense querying the vertical FOV of a camera other than perspective ! There might be a logic error somewhere !
		MOE_DEBUG_ASSERT(m_projectionType == CameraProjection::Perspective);

		return m_cameraData.m_perspective.m_fovY;
	}


	void Camera::SetFoVY(Degs_f newFovY)
	{

		if (newFovY < m_minFoVY)
			newFovY = m_minFoVY;
		else if (newFovY > m_maxFoVY)
		{
			newFovY = m_maxFoVY;
		}

		m_projectionType = CameraProjection::Perspective;
		m_cameraData.m_perspective.m_fovY = newFovY;
		// Fov has changed - recompute proj matrix
		ComputeProjectionMatrices();
	}


	void Camera::SetAspectRatio(float newAspect)
	{
		m_projectionType = CameraProjection::Perspective;
		m_cameraData.m_perspective.m_aspectRatio = newAspect;
		// Aspect has changed - recompute proj matrix
		ComputeProjectionMatrices();
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
		ComputeProjectionMatrices();
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
		ComputeProjectionMatrices();
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


	const Transform& Camera::LookAt(const Vec3& lookatPointWorld, const Vec3& upWorld)
	{
		/* TODO as it's a bit of a code smell :
		* to compute camera transform, when using lookat, LookatMatrix actually computes a VIEW matrix !
		* So the actual transform matrix is the inverse of the matrix returned by LookAt. But RecomputeViewMatrices inverses the transform AGAIN !
		* There has to be a better way.
		*/
		const Vec3 camPos = GetTransform().Matrix().GetTranslation();

		SetTransform(Transform(Mat4::LookAtMatrix(camPos, lookatPointWorld, upWorld)).Matrix().GetInverse());

		RecomputeViewMatrices();

		m_cameraFront = (lookatPointWorld - camPos);
		m_cameraUp = upWorld;

		return GetTransform();
	}


	const Transform& Camera::LookAt(const Vec3& cameraPosWorld, const Vec3& lookatPointWorld, const Vec3& upWorld)
	{
		/* TODO as it's a bit of a code smell :
		 * to compute camera transform, when using lookat, LookatMatrix actually computes a VIEW matrix !
		 * So the actual transform matrix is the inverse of the matrix returned by LookAt. But RecomputeViewMatrices inverses the transform AGAIN !
		 * There has to be a better way.
		 */
		SetTransform(Transform(Mat4::LookAtMatrix(cameraPosWorld, lookatPointWorld, upWorld)).Matrix().GetInverse());

		RecomputeViewMatrices();

		m_cameraFront = (lookatPointWorld - cameraPosWorld);
		m_cameraUp = upWorld;

		return GetTransform();
	}


	void Camera::UpdateCameraVectors(float pitch, float yaw)
	{
		m_pitch = Degs_f(pitch);

		if (m_pitch > m_pitchThreshold)
			m_pitch = m_pitchThreshold;
		if (m_pitch < -m_pitchThreshold)
			m_pitch = -m_pitchThreshold;

		m_yaw = Degs_f(yaw);

		const Rads_f yawRads{ m_yaw };
		const Rads_f pitchRads{ m_pitch };

		// Calculate the new Front vector
		m_cameraFront = Vec3{
			cosf(yawRads) * cosf(pitchRads),
			sinf(pitchRads),
			sinf(yawRads) * cosf(pitchRads)
		}.GetNormalized();


		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		const Vec3 worldUp{0, 1, 0};
		m_cameraRight = m_cameraFront.Cross(worldUp).GetNormalized();
		m_cameraUp = m_cameraRight.Cross(m_cameraFront).GetNormalized();
	}


	void Camera::RecomputeViewMatrices()
	{
		m_matrices.m_view = m_transform.Matrix().GetInverse();
		m_matrices.m_viewProj = m_matrices.m_proj * m_matrices.m_view;

		m_matricesDataPtr->m_view = m_matrices.m_view; // TODO : cleanup

		m_matricesDataPtr->m_viewProj = m_matrices.m_viewProj; // TODO : cleanup

		if (m_parentSystem)
			m_parentSystem->FlagUpdateNeeded();
	}


	void Camera::ComputeProjectionMatrices()
	{
		switch (m_projectionType)
		{
		case CameraProjection::Orthographic:
			m_matrices.m_proj = Mat4::Orthographic(
				m_cameraData.m_ortho.m_left, m_cameraData.m_ortho.m_right,
				m_cameraData.m_ortho.m_bottom, m_cameraData.m_ortho.m_top,
				m_cameraData.m_ortho.m_near, m_cameraData.m_ortho.m_far);

			m_matricesDataPtr->m_proj = m_matrices.m_proj; // TODO : cleanup
			break;
		case CameraProjection::Perspective:
			m_matrices.m_proj = Mat4::Perspective(
				Rads_f(m_cameraData.m_perspective.m_fovY), m_cameraData.m_perspective.m_aspectRatio,
				m_cameraData.m_perspective.m_near, m_cameraData.m_perspective.m_far);

			m_matricesDataPtr->m_proj = m_matrices.m_proj; // TODO : cleanup
			break;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unsupported camera projection type");
			return;
		}

		// Projection matrix has changed : recompute view proj matrix too
		m_matrices.m_viewProj = m_matrices.m_proj * m_matrices.m_view;

		m_matricesDataPtr->m_viewProj = m_matrices.m_viewProj; // TODO : cleanup

		if (m_parentSystem)
			m_parentSystem->FlagUpdateNeeded();
	}
}
