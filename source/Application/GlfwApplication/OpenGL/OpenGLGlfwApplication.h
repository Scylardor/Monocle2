// Monocle Game Engine source files - Alexandre Baron

#pragma once

#if defined(MOE_GLFW) && defined(MOE_OPENGL)

#include "Core/Preprocessor/moeDLLVisibility.h"

#include "Graphics/Renderer/OpenGL/OpenGLRenderer.h"

#include "Application/GlfwApplication/BaseGlfwApplication.h"

#include "Application/GlfwApplication/OpenGL/OpenGLGlfwAppDescriptor.h"

namespace moe
{
	/**
	 * \brief Application class that uses an OpenGL context.
	 * Can be parameterized about major/minor OpenGL versions and needed OpenGL profile type (core/compat).
	 */
	class MOE_DLL_API OpenGLGlfwApplication : public BaseGlfwApplication
	{
	public:
		OpenGLGlfwApplication(const struct OpenGLGlfwAppDescriptor& appDesc);
		~OpenGLGlfwApplication() = default;


		const IGraphicsRenderer&	GetRenderer() const override
		{
			return m_renderer;
		}
		IGraphicsRenderer&			MutRenderer() override
		{
			return m_renderer;
		}

	protected:

		OpenGLRenderer	m_renderer;
	};
}

#endif // defined(MOE_GLFW) && defined(MOE_OPENGL)