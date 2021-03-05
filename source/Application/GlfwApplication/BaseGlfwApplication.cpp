// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_GLFW

#include "BaseGlfwApplication.h"
#include "Application/AppDescriptor/AppDescriptor.h"

#include <GLFW/glfw3.h>


moe::BaseGlfwApplication::BaseGlfwApplication()
{
	SetInitialized(glfwInit());
}


moe::BaseGlfwApplication::~BaseGlfwApplication()
{
	glfwDestroyWindow(m_window);

	glfwTerminate();
}


::GLFWwindow* moe::BaseGlfwApplication::CreateGlfwWindow(const AppDescriptor& appDesc)
{
	if (false == MOE_ASSERT(m_initialized))
	{
		return nullptr;
	}

	glfwWindowHint(GLFW_RESIZABLE, appDesc.m_resizableWindow);

	glfwWindowHint(GLFW_SAMPLES, appDesc.m_numSamples);

	m_window = glfwCreateWindow(appDesc.m_windowWidth, appDesc.m_windowHeight, appDesc.m_windowTitle, nullptr, nullptr);
	if (m_window == nullptr)
	{
		CheckGLFWError();
		MOE_ERROR(moe::ChanWindowing, "Failed to create GLFW window.");
	}
	else
	{
		/* TODO : parameterize that with the app descriptor */
		glfwSetWindowUserPointer(m_window, this);

		glfwSetKeyCallback(m_window, KeyCallback);

		glfwSetCursorPosCallback(m_window, MouseMoveCallback);

		glfwSetScrollCallback(m_window, MouseScrollCallback);

		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	return m_window;
}

void moe::BaseGlfwApplication::PollInputEvents()
{
	glfwPollEvents();
}

void moe::BaseGlfwApplication::SwapBuffers()
{
	glfwSwapBuffers(m_window);
}


bool moe::BaseGlfwApplication::WindowIsOpened() const
{
	return !glfwWindowShouldClose(m_window);
}


float moe::BaseGlfwApplication::GetApplicationTimeSeconds() const
{
	return (float)glfwGetTime();
}


int moe::BaseGlfwApplication::CheckGLFWError() const
{
	const char* description;
	int code = glfwGetError(&description);

	if (description)
	{
		MOE_DEBUG_ASSERT(false);
		MOE_ERROR(moe::ChanWindowing, "GLFW error: %s (code %i)", description, code);
	}

	return code;
}


std::pair<float, float> moe::BaseGlfwApplication::GetMouseCursorPosition()
{
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);

	return {(float)xpos, (float)ypos};
}


void moe::BaseGlfwApplication::InitializeGraphicsContext()
{
	glfwMakeContextCurrent(GetGlfwWindow());
}


void moe::BaseGlfwApplication::ReleaseGraphicsContext()
{
	glfwMakeContextCurrent(nullptr);
}


void moe::BaseGlfwApplication::KeyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
	BaseGlfwApplication* me = static_cast<BaseGlfwApplication*>(glfwGetWindowUserPointer(window));

	me->m_inputMgr.CallKeyboardInputCallback(key, action);
}


void moe::BaseGlfwApplication::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	BaseGlfwApplication* me = static_cast<BaseGlfwApplication*>(glfwGetWindowUserPointer(window));

	me->m_inputMgr.CallMouseMoveCallbacks(xpos, ypos);
}


void moe::BaseGlfwApplication::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	BaseGlfwApplication* me = static_cast<BaseGlfwApplication*>(glfwGetWindowUserPointer(window));

	me->m_inputMgr.CallMouseScrollCallbacks(xoffset, yoffset);
}



#endif // MOE_GLFW
