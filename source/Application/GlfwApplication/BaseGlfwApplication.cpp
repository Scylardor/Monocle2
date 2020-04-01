// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_GLFW

#include "BaseGlfwApplication.h"
#include "Application/AppDescriptor/AppDescriptor.h"



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


#endif // MOE_GLFW
