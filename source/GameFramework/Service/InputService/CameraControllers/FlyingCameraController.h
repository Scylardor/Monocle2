#pragma once
#include <variant>

#include "Core/Delegates/EventDelegateID.h"
#include "Math/Matrix.h"
#include "Math/Vec3.h"
#include "Math/Angles/Angles.h"


namespace moe
{
	struct OrthographicProjectionData;
	struct PerspectiveProjectionData;
	class IGraphicsSurface;
	class RenderScene;
	class InputService;

	class IController
	{
	public:
		virtual ~IController() = default;

		virtual void	Update(float dt) = 0;
	};


	class ACameraController : public IController
	{
	public:

		ACameraController(RenderScene& cameraScene) :
			m_cameraScene(&cameraScene)
		{}



	protected:


		using ViewID = uint32_t;
		ViewID					m_viewID{};
		RenderScene*			m_cameraScene = nullptr;
	};





	class FlyingCameraController : public ACameraController
	{
	public:

		FlyingCameraController(RenderScene& cameraScene, Vec3 const& pos, Vec3 const& target, Vec3 const& up, PerspectiveProjectionData const& perspective);
		FlyingCameraController(RenderScene& cameraScene, Vec3 const& pos, Vec3 const& target, Vec3 const& up, OrthographicProjectionData const& ortho);

		void	Update(float dt) override;


	private:

		enum class CameraMoveState : char
		{
			Forward = 1 << 0,
			Backward = 1 << 1,
			Left = 1 << 2,
			Right = 1 << 3,
			None = 0
		};


		[[nodiscard]] bool	IsMoving(CameraMoveState state) const
		{
			return (((char)m_movingState & (char)state) == (char)state);
		}


		[[nodiscard]] bool	IsMoving() const
		{
			return m_movingState != CameraMoveState::None;
		}

		void	StartMoving(CameraMoveState direction)
		{
			m_movingState = CameraMoveState((char)m_movingState | (char)direction);
		}

		void	StopMoving(CameraMoveState direction)
		{
			m_movingState = CameraMoveState((char)m_movingState & ~(char)direction);
		}


		void	SetupInputDelegates();

		void	OnCursorMove(double newX, double newY);

		void	RecomputeCameraVectors(float xDelta, float yDelta);

		void	MoveForward();
		void	MoveBackward();
		void	StrafeLeft();
		void	StrafeRight();

		void	StopMoveForward();
		void	StopMoveBackward();
		void	StopStrafeLeft();
		void	StopStrafeRight();

		void	OnScroll(double xpos, double ypos);

		Vec3	m_cameraPos{ Vec3::ZeroVector() };
		Vec3	m_cameraFront{ 0, 0, -1 };
		Vec3	m_cameraRight{ 1, 0, 0 };
		Vec3	m_cameraUp{ 0, 1, 0 };
		Degs_f	m_yaw{ -90.0f };
		Degs_f	m_pitch{ 0.f };
		float	m_pitchThreshold{ 89.f };

		float	m_lastCursorX = NAN;
		float	m_lastCursorY = NAN;

		float	m_rotationSensitivity = 0.1f;

		float	m_dollySpeed = 5;
		float	m_truckSpeed = 5;

		// TODO: should be Degs_f !
		float	m_minZoomFOVy = 1.f;
		float	m_maxZoomFOVy = 45.f;

		bool	m_needsViewUpdate = false;

		EventDelegateID	m_mouseMoveDlgt;

		EventDelegateID	m_mouseScrollDlgt;

		EventDelegateID	m_moveForwardDlgt;
		EventDelegateID	m_stopMoveForwardDlgt;

		EventDelegateID	m_moveBackwardDlgt;
		EventDelegateID	m_stopMoveBackwardDlgt;

		EventDelegateID	m_strafeLeftDlgt;
		EventDelegateID	m_stopStrafeLeftDlgt;

		EventDelegateID	m_strafeRightDlgt;
		EventDelegateID	m_stopStrafeRightDlgt;

		CameraMoveState	m_movingState{ CameraMoveState::None };
	};

}
