// Monocle Game Engine source files - Alexandre Baron

#pragma once
#if defined(MOE_GLFW ) && defined(MOE_OPENGL)
#include "GLFWService.h"

#include "Graphics/RHI/OpenGL/OGL4RHI.h"

#include <GLFW/glfw3.h>

namespace moe
{
	class OpenGLGLFWService final : public GLFWService
	{
	public:

		OpenGLGLFWService(Engine& engine);

		~OpenGLGLFWService() = default;


		moe::IWindow* CreateWindow() override;

		static OpenGL4RHI::GLLoaderFunction	GetGLLoaderFunction()
		{
			return (OpenGL4RHI::GLLoaderFunction) glfwGetProcAddress;
		}

	};

}


#endif