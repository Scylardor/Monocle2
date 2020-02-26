// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_GLFW

#include "GlfwGraphicsContext.h"
#include "Graphics/Context/GraphicsContextDescriptor.h"

#include <GLFW/glfw3.h>

void moe::GlfwGraphicsContext::InitPreWindowCreation(const moe::GraphicsContextDescriptor& contextDesc)
{
	int major{ 0 }, minor{ 0 };

	// If versions are default, use OpenGL 4.5.
	if (contextDesc.HasDefaultApiVersion())
	{
		major = 4;
		minor = 5;
	}
	else
	{
		major = contextDesc.API_MajorVersion;
		minor = contextDesc.API_MinorVersion;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


void moe::GlfwGraphicsContext::InitPostWindowCreation(const moe::GraphicsContextDescriptor& contextDesc, GenericWindowHandle windowHandle)
{
	glfwMakeContextCurrent((GLFWwindow*)windowHandle);
}


#endif // MOE_GLFW