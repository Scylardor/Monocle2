// Monocle Game Engine source files - Alexandre Baron

#if defined(MOE_GLFW )
#include "GLFWWindow.h"

#include "GameFramework/Service/ConfigService/ConfigService.h"
#include "GameFramework/Engine/Engine.h"

namespace moe
{


	GLFWWindow::~GLFWWindow()
	{
		if (HasWindow())
		{
			// reenable the cursor before destroying, otherwise on Windows,
			// after breaking into the debugger, a disabled cursor will stay locked in the old window 'frame' after destruction.
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			glfwDestroyWindow(m_window);
		}
	}


	void GLFWWindow::PollInputs()
	{
		glfwPollEvents();

		m_openState = glfwWindowShouldClose(m_window) ? WindowState::Closed : WindowState::Opened;
	}


	void GLFWWindow::SwapBuffers()
	{
		glfwSwapBuffers(m_window);
	}


	std::pair<int, int> GLFWWindow::GetDimensions() const
	{
		MOE_ASSERT(m_window);

		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);
		return { width, height };
	}


	void GLFWWindow::SetWindowHints(ConfigSection const& windowConfig)
	{
		auto resizable = windowConfig.GetBool("resizable").value_or(true);

		auto samples = windowConfig.GetUint("framebuffer.msaa").value_or(4);

		glfwWindowHint(GLFW_RESIZABLE, resizable);

		glfwWindowHint(GLFW_SAMPLES, samples);
	}


	bool GLFWWindow::Create(ConfigSection const& windowConfig)
	{
		auto width = windowConfig.GetUint("width").value_or(800);
		auto height = windowConfig.GetUint("height").value_or(600);
		char const* title = windowConfig.GetString("title").value_or("Default Title");

		std::string_view const videoMode = windowConfig.GetString("video_mode").value_or("");

		GLFWmonitor* monitor = nullptr;
		if (videoMode == "fullscreen")
		{
			monitor = glfwGetPrimaryMonitor();
		}
		else if (videoMode == "borderless")
		{
			monitor = glfwGetPrimaryMonitor();

			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			width = mode->width;
			height = mode->height;
		}

		m_window = glfwCreateWindow(width, height, title,  monitor, nullptr);

		bool ok = HasWindow();

		if (ok)
		{
			glfwSetWindowUserPointer(m_window, this);
			glfwSetFramebufferSizeCallback(m_window, WindowFramebufferResizeCallback);
			glfwSetKeyCallback(m_window, WindowKeyCallback);
			glfwSetCursorPosCallback(m_window, WindowCursorPosCallback);
			glfwSetMouseButtonCallback(m_window, WindowMouseCallback);
			glfwSetScrollCallback(m_window, WindowMouseScrollCallback);
			ok &= PostCreate(windowConfig);
			MOE_DEBUG_ASSERT(ok);
		}

		return ok;
	}


	bool GLFWWindow::PostCreate(ConfigSection const& windowConfig)
	{
		MOE_ASSERT(HasWindow());

		std::string_view const cursor = windowConfig.GetString("cursor").value_or("");

		if (cursor == "disabled")
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (cursor == "hidden")
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		auto enableRawMouseMotion = windowConfig.GetBool("raw_mouse_motion").value_or(false);
		if (glfwRawMouseMotionSupported()&& enableRawMouseMotion)
		{
			glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		// vsync is enabled by default
		bool const vsync = windowConfig.GetBool("vsync").value_or(true);

		// swap interval of 1 means "wait 1 vblank" so vsync is ON, 0 means vsync OFF
		// there's not really any point in waiting for more vblanks than creating more latency, so stick to 1 or 0
		glfwSwapInterval((int)vsync);

		return true;
	}

	void GLFWWindow::WindowFramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		GLFWWindow* myWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
		myWindow->OnSurfaceResized(width, height);
	}


	void GLFWWindow::WindowCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		GLFWWindow* myWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
		myWindow->CursorPositionUpdateEvent.Broadcast(xpos, ypos);
	}

	void GLFWWindow::WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		GLFWWindow* myWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
		myWindow->KeyboardKeyEvent.Broadcast(key, scancode, ButtonPressedState(action), mods);
	}

	void GLFWWindow::WindowMouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		GLFWWindow* myWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
		myWindow->MouseButtonUpdateEvent.Broadcast(MouseButton(button), ButtonPressedState(action), mods);
	}


	void GLFWWindow::WindowMouseScrollCallback(GLFWwindow* window, double xscroll, double yscroll)
	{
		GLFWWindow* myWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
		myWindow->MouseScrollUpdateEvent.Broadcast(xscroll, yscroll);
	}

}


#endif