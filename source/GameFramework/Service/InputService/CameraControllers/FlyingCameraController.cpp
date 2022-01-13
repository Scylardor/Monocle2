#include "FlyingCameraController.h"

#include "GameFramework/Service/RenderService/RenderScene/RenderScene.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Engine/Engine.h"

#include "GameFramework/Service/InputService/InputService.h"

namespace moe
{
	FlyingCameraController::FlyingCameraController(RenderScene& cameraScene, Vec3 const& pos, Vec3 const& target, Vec3 const& up, PerspectiveProjectionData const& perspective) :
		ACameraController(cameraScene),
		m_cameraPos(pos),
		m_cameraUp(up)
	{
		Mat4 viewMatrix = Mat4::LookAtMatrix(pos, target, up);
		m_viewID = cameraScene.AddView(Mat4::Identity(), perspective);

		SetupInputDelegates();

		RecomputeCameraVectors(0, 0);

	}

	FlyingCameraController::FlyingCameraController(RenderScene& cameraScene, Vec3 const& pos, Vec3 const& target,
		Vec3 const& up, OrthographicProjectionData const& ortho) :
		ACameraController(cameraScene),
		m_cameraPos(pos),
		m_cameraUp(up)
	{
		Mat4 viewMatrix = Mat4::LookAtMatrix(pos, target, up);
		m_viewID = cameraScene.AddView(viewMatrix, ortho);

		SetupInputDelegates();
	}


	void FlyingCameraController::Update(float dt)
	{
		// TODO: I think due to transformations the vector is ALMOST unit, this assert breaks, maybe try an Approx() type of thing.
		//MOE_DEBUG_ASSERT(m_cameraFront.IsUnit());
		//MOE_DEBUG_ASSERT(m_cameraRight.IsUnit());

		if (IsMoving(CameraMoveState::Forward))
		{
			m_cameraPos += m_cameraFront * (m_dollySpeed * dt);
		}

		if (IsMoving(CameraMoveState::Backward))
		{
			m_cameraPos -= m_cameraFront * (m_dollySpeed * dt);
		}

		if (IsMoving(CameraMoveState::Left))
		{
			m_cameraPos -= m_cameraRight * (m_truckSpeed * dt);
		}

		if (IsMoving(CameraMoveState::Right))
		{
			m_cameraPos += m_cameraRight * (m_truckSpeed * dt);
		}

		if (IsMoving() || m_needsViewUpdate)
		{
			Mat4 viewMatrix = Mat4::LookAtMatrix(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
			m_cameraScene->UpdateViewMatrix(m_viewID, viewMatrix);
			m_needsViewUpdate = false;
		}
	}


	void FlyingCameraController::SetupInputDelegates()
	{
		// Obtain the associated input service to plug in the camera controls.
		auto* svcInput = m_cameraScene->MutRenderer()->MutRenderService()->EditEngine()->EditService<InputService>();
		m_mouseMoveDlgt = svcInput->MutCursorUpdateEvent().Add<FlyingCameraController, &FlyingCameraController::OnCursorMove>(this);

		m_moveForwardDlgt = svcInput->MutKeyActionEvent('W', ButtonPressedState::Held)
			.Add<FlyingCameraController, &FlyingCameraController::MoveForward>(this);
		m_stopMoveForwardDlgt = svcInput->MutKeyActionEvent('W', ButtonPressedState::Released)
			.Add<FlyingCameraController, &FlyingCameraController::StopMoveForward>(this);

		m_moveBackwardDlgt = svcInput->MutKeyActionEvent('S', ButtonPressedState::Held)
			.Add<FlyingCameraController, &FlyingCameraController::MoveBackward>(this);
		m_stopMoveBackwardDlgt = svcInput->MutKeyActionEvent('S', ButtonPressedState::Released)
			.Add<FlyingCameraController, &FlyingCameraController::StopMoveBackward>(this);

		m_strafeLeftDlgt = svcInput->MutKeyActionEvent('A', ButtonPressedState::Held)
			.Add<FlyingCameraController, &FlyingCameraController::StrafeLeft>(this);
		m_stopStrafeLeftDlgt = svcInput->MutKeyActionEvent('A', ButtonPressedState::Released)
			.Add<FlyingCameraController, &FlyingCameraController::StopStrafeLeft>(this);

		m_strafeRightDlgt = svcInput->MutKeyActionEvent('D', ButtonPressedState::Held)
			.Add<FlyingCameraController, &FlyingCameraController::StrafeRight>(this);
		m_stopStrafeRightDlgt = svcInput->MutKeyActionEvent('D', ButtonPressedState::Released)
			.Add<FlyingCameraController, &FlyingCameraController::StopStrafeRight>(this);

		m_mouseScrollDlgt = svcInput->MutMouseScrollEvent()
			.Add<FlyingCameraController, &FlyingCameraController::OnScroll>(this);
	}


	void FlyingCameraController::OnCursorMove(double newX, double newY)
	{
		float nx = (float)newX, ny = (float)newY;

		if (std::isnan(m_lastCursorX) && std::isnan(m_lastCursorY))
		{
			m_lastCursorX = nx;
			m_lastCursorY = ny;
		}

		float horizDelta = nx - m_lastCursorX;
		float vertiDelta = m_lastCursorY - ny; // reversed since y-coordinates go from bottom to top

		m_lastCursorX = nx;
		m_lastCursorY = ny;

		horizDelta *= m_rotationSensitivity;
		vertiDelta *= m_rotationSensitivity;

		RecomputeCameraVectors(horizDelta, vertiDelta);

		MOE_LOG("Camera pos %f %f %f\n", m_cameraPos.x(), m_cameraPos.y(), m_cameraPos.z());
	}


	void FlyingCameraController::RecomputeCameraVectors(float xDelta, float yDelta)
	{
		m_yaw += xDelta;
		m_pitch += yDelta;

		// make sure that when pitch is out of bounds, screen doesn't get flipped

		m_pitch = std::clamp(m_pitch, -m_pitchThreshold, m_pitchThreshold);

		// update Front, Right and Up Vectors using the updated Euler angles
		// calculate the new Front vector
		const Rads_f yawRads{ m_yaw };
		const Rads_f pitchRads{ m_pitch };

		// Calculate the new Front vector
		m_cameraFront = Vec3{
			cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch)),
			sinf(glm::radians(m_pitch)),
			sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch))
		}.GetNormalized();

		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		static const Vec3 worldUp{ 0, 1, 0 };
		m_cameraRight = m_cameraFront.Cross(worldUp).GetNormalized();
		m_cameraUp = m_cameraRight.Cross(m_cameraFront).GetNormalized();

		m_needsViewUpdate = true;
	}


	void FlyingCameraController::MoveForward()
	{
		StartMoving(CameraMoveState::Forward);
	}

	void FlyingCameraController::MoveBackward()
	{
		StartMoving(CameraMoveState::Backward);
	}

	void FlyingCameraController::StrafeLeft()
	{
		StartMoving(CameraMoveState::Left);
	}

	void FlyingCameraController::StrafeRight()
	{
		StartMoving(CameraMoveState::Right);
	}

	void FlyingCameraController::StopMoveForward()
	{
		StopMoving(CameraMoveState::Forward);
	}

	void FlyingCameraController::StopMoveBackward()
	{
		StopMoving(CameraMoveState::Backward);
	}

	void FlyingCameraController::StopStrafeLeft()
	{
		StopMoving(CameraMoveState::Left);
	}

	void FlyingCameraController::StopStrafeRight()
	{
		StopMoving(CameraMoveState::Right);
	}

	void FlyingCameraController::OnScroll(double /*xpos*/, double ypos)
	{
		m_cameraScene->OffsetViewFoVy(m_viewID, (float) -ypos, m_minZoomFOVy, m_maxZoomFOVy);
	}
}
