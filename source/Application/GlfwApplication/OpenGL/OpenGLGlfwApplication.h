// Monocle Game Engine source files - Alexandre Baron

#pragma once

#if defined(MOE_GLFW) && defined(MOE_OPENGL)

#include "Monocle_Application_Export.h"

#include "Graphics/Renderer/OpenGL/OpenGLRenderer.h"

#include "Application/GlfwApplication/BaseGlfwApplication.h"

#include "Application/GlfwApplication/OpenGL/OpenGLGlfwAppDescriptor.h"

namespace moe
{
	/**
	 * \brief Application class that uses an OpenGL context.
	 * Can be parameterized about major/minor OpenGL versions and needed OpenGL profile type (core/compat).
	 */
	class OpenGLGlfwApplication : public BaseGlfwApplication
	{
	public:
		Monocle_Application_API OpenGLGlfwApplication(const struct OpenGLGlfwAppDescriptor& appDesc);
		Monocle_Application_API virtual ~OpenGLGlfwApplication() override;

		OpenGLGlfwApplication(const OpenGLGlfwApplication&) = delete;


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