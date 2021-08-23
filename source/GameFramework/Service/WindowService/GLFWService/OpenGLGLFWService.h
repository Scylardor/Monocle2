// Monocle Game Engine source files - Alexandre Baron

#pragma once
#if defined(MOE_GLFW ) && defined(MOE_OPENGL)
#include "GLFWService.h"

namespace moe
{
	class OpenGLGLFWService final : public GLFWService
	{
	public:

		OpenGLGLFWService(Engine& engine);

		~OpenGLGLFWService() = default;


		std::unique_ptr<moe::IWindow> CreateWindow() override;


	};

}


#endif