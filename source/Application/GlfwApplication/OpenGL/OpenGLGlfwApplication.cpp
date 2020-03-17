// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_GLFW

#include <glad/glad.h> // Keep on top to avoid glad/glfw conflicts (glad always wants to be first header included)

#include "OpenGLGlfwApplication.h"

#include "Application/GlfwApplication/OpenGL/OpenGLGlfwAppDescriptor.h"



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
		if (myWindow != nullptr)
		{
			glfwMakeContextCurrent(myWindow);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				MOE_ERROR(moe::ChanWindowing, "Failed to initialize GLAD OpenGL loader.");

				m_initialized = false;
			}
		}
	}
}


#endif // MOE_GLFW