// Monocle Game Engine source files - Alexandre Baron

#pragma once
#if defined(MOE_GLFW) && defined(MOE_OPENGL)
#include "GLFWWindow.h"

namespace moe
{
	class OpenGLGLFWWindow final : public GLFWWindow
	{
	public:
		OpenGLGLFWWindow() = default;

		void				SetWindowHints(ConfigSection const& windowConfig) override;


	protected:


		bool	PostCreate(ConfigSection const& windowConfig) override;

	};

}


#endif