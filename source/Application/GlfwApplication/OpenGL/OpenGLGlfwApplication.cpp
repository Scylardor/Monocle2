// Monocle Game Engine source files - Alexandre Baron

#if defined(MOE_GLFW) && defined(MOE_OPENGL)

#include "OpenGLGlfwApplication.h"

#include "Application/GlfwApplication/OpenGL/OpenGLGlfwAppDescriptor.h"

#include <GLFW/glfw3.h>



moe::OpenGLGlfwApplication::OpenGLGlfwApplication(const struct OpenGLGlfwAppDescriptor& appDesc) :
	BaseGlfwApplication()
{
	if (m_initialized)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, appDesc.m_contextVersionMajor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, appDesc.m_contextVersionMinor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, appDesc.m_openGLProfile);

		//TODO: uncomment on Mac OS
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		GLFWwindow* myWindow = CreateGlfwWindow(appDesc);
		m_initialized = (myWindow != nullptr);
	}

	if (false == m_initialized)
	{
		MOE_ERROR(moe::ChanWindowing, "Failed to create GLFW window.");
	}
	else
	{
		glfwMakeContextCurrent(GetGlfwWindow());

		m_initialized = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (false == m_initialized)
		{
			MOE_ERROR(moe::ChanWindowing, "Failed to initialize GLAD OpenGL loader.");
		}
		else
		{

		}
	}
}


#endif // MOE_GLFW

#endif // defined(MOE_GLFW) && defined(MOE_OPENGL)
