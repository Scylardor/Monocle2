// Monocle Game Engine source files - Alexandre Baron

#pragma once
#if defined(MOE_GLFW )
#include "GameFramework/Service/WindowService/GLFWService/GLFWService.h"

namespace moe
{
	class GLFWService;

	class GLFWWindow : public IWindow
	{
	public:

		GLFWWindow(GLFWService& creator);

		~GLFWWindow();


	private:

		GLFWwindow* m_window = nullptr;
	};



#ifdef MOE_OPENGL

	class OpenGLGLFWWindow : public GLFWWindow
	{
	public:
		OpenGLGLFWWindow(GLFWService& creator);

	};

#endif
}


#endif