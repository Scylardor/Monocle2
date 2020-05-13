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

	m_window = glfwCreateWindow(appDesc.m_windowWidth, appDesc.m_windowHeight, appDesc.m_windowTitle, nullptr, nullptr);
	if (m_window == nullptr)
	{
		MOE_ERROR(moe::ChanWindowing, "Failed to create GLFW window.");
	}

	/* TODO : parameterize that with the app descriptor */
	glfwSetWindowUserPointer(m_window, this);

	glfwSetKeyCallback(m_window, KeyCallback);

	glfwSetCursorPosCallback(m_window, MouseMoveCallback);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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


std::pair<float, float> moe::BaseGlfwApplication::GetMouseCursorPosition()
{
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);

	return {(float)xpos, (float)ypos};
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


#endif // MOE_GLFW
