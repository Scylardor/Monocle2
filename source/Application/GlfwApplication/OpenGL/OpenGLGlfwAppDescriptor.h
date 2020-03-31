// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLFW

#include "Core/Preprocessor/moeDLLVisibility.h"

#include "Application/AppDescriptor/AppDescriptor.h"

#include <GLFW/glfw3.h> // GLFW_OPENGL_CORE_PROFILE


namespace moe
{
	/**
	 * \brief A descriptor for a GLFW application OpenGL context.
	 * Can be parameterized about major/minor OpenGL versions and needed OpenGL profile type (core/compat).
	 */
	struct OpenGLGlfwAppDescriptor : public AppDescriptor
	{
	public:
		using AppDescriptor::AppDescriptor;

		OpenGLGlfwAppDescriptor() = default;
		~OpenGLGlfwAppDescriptor() = default;

		int	m_contextVersionMajor = 4;
		int	m_contextVersionMinor = 5;
		int	m_openGLProfile = GLFW_OPENGL_CORE_PROFILE;
	};
}

#endif // MOE_GLFW