#pragma once

#include "Application/GlfwApplication/OpenGL/OpenGLGlfwApplication.h"

namespace moe
{


class TestApplication final : public moe::OpenGLGlfwApplication
{
public:

	TestApplication(const moe::OpenGLGlfwAppDescriptor& appDesc);
	~TestApplication() = default;

	void	Run();

	// Test Basics
	void	TestSceneGraph();
	void	Test3DPlane();
	void	TestCubeWorld();
	void	TestMultiLights();

	// Test Advanced 1
	void	TestVisualizeDepthBuffer();
	void	TestStencilBuffer();
	void	TestBlending();
	void	TestFramebuffer();
	void	TestCubemap();

	// Others
	void	CameraMoveForward();
	void	CameraMoveBackwards();
	void	CameraMoveStrafeLeft();
	void	CameraMoveStrafeRight();

	void	OrientCameraWithMouse(double xpos, double ypos);

	void	CameraZoomMouseScroll(double xpos, double ypos);

private:
	using AppLogger = moe::StdLogger<moe::NoFilterPolicy, moe::DebuggerFormatPolicy, moe::IdeWritePolicy>;

	AppLogger	m_logger;

	Camera*	m_currentCamera = nullptr;
	float	m_deltaTime{0.f};
	float	m_lastFrame = 0.0f; // Time of last frame
	const float m_cameraSpeed{5.F};


	bool		m_moveForward = false;
	bool		m_moveBackward = false;
	bool		m_strafeLeft = false;
	bool		m_strafeRight = false;
	float		m_lastX = 0;
	float		m_lastY = 0;
	float		m_cameraYaw = -90.f;
	float		m_cameraPitch = 0.f;
	const float m_mouseSensitivity = 0.05f;

	class LightObject*	m_flashLight = nullptr;
};

}
