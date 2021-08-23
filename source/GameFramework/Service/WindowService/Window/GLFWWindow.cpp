// Monocle Game Engine source files - Alexandre Baron

#if defined(MOE_GLFW )
#include "GLFWWindow.h"

#include "GameFramework/Service/ConfigService/ConfigService.h"
#include "GameFramework/Engine/Engine.h"

namespace moe
{

	GLFWWindow::GLFWWindow(GLFWService& creator)
	{
		auto const* Config = creator.EditEngine()->GetService<ConfigService>();
		MOE_ASSERT(Config != nullptr);

		auto width = Config->GetUint("window.width");
		auto width2 = Config->GetUint("window.pouet");
		auto width3 = Config->GetUint("height");
		auto width4 = Config->GetUint("window.framebuffer.msaa");

	}

	GLFWWindow::~GLFWWindow()
	{
		if (m_window != nullptr)
			glfwDestroyWindow(m_window);
	}


#ifdef MOE_OPENGL

	OpenGLGLFWWindow::OpenGLGLFWWindow(GLFWService& creator) :
		GLFWWindow(creator)
	{

	}

#endif
}


#endif