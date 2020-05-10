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


#endif // MOE_GLFW
