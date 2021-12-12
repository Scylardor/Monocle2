// Monocle Game Engine source files - Alexandre Baron

#if defined(MOE_GLFW ) && defined(MOE_OPENGL)
#include <glad/glad.h> // Keep it first or it complains
#include "OpenGLGLFWWindow.h"
#include "GameFramework/Service/ConfigService/ConfigService.h"


namespace moe
{
	void OpenGLGLFWWindow::SetWindowHints(ConfigSection const& windowConfig)
	{
		// there is no way to ask the driver for a context of the highest supported version,
		// if there is no configuration, try to load 4.5 by default. TODO: this should be changed
		auto GLmajor = windowConfig.GetUint("GL_major");
		if (GLmajor.has_value())
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLmajor.value());
		else
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);


		auto GLminor = windowConfig.GetUint("GL_minor");
		if (GLminor.has_value())
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLminor.value());
		else
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		// From the docs :
		// If requesting an OpenGL version below 3.2, GLFW_OPENGL_ANY_PROFILE must be used. If OpenGL ES is requested, this hint is ignored.
		// If the 1.0 special version is used, we use core (it's supposed to mean "highest available".
		bool const higherThan32 = (GLmajor >= 3u && (GLminor >= (GLmajor == 3u ? 2u : 0u)));
		bool const enableCoreProfile = higherThan32 || (!GLmajor.has_value() && !GLminor.has_value());
		if (enableCoreProfile)
		{
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}
		else
		{
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
		}

		// Activate the debug context on the Debug target
#ifdef MOE_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

		// The MacOS only supports forward-compatible core profile contexts for OpenGL versions 3.2 and later.
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		GLFWWindow::SetWindowHints(windowConfig);
	}


	bool OpenGLGLFWWindow::PostCreate(ConfigSection const& windowConfig)
	{
		glfwMakeContextCurrent(EditWindow());

		bool ok = GLFWWindow::PostCreate(windowConfig);

		return ok;
	}
}


#endif