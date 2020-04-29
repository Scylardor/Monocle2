// Monocle Game Engine source files - Alexandre Baron

#pragma once

#if defined(MOE_GLFW) && defined(MOE_OPENGL)

#include "Monocle_Application_Export.h"

#include "Application/AppDescriptor/AppDescriptor.h"

namespace moe
{
	/**
	 * \brief A descriptor for a GLFW application OpenGL context.
	 * Can be parameterized about major/minor OpenGL versions and needed OpenGL profile type (core/compat).
	 */
	struct  OpenGLGlfwAppDescriptor : public AppDescriptor
	{
	public:
		Monocle_Application_API OpenGLGlfwAppDescriptor(Width_t w, Height_t h, const char* windowTitle = "Monocle Application", const char* iconPath = nullptr, bool resizableWin = false);

		Monocle_Application_API ~OpenGLGlfwAppDescriptor() = default;

		int	m_contextVersionMajor = 4;
		int	m_contextVersionMinor = 5;
		int	m_openGLProfile;
	};
}

#endif // defined(MOE_GLFW) && defined(MOE_OPENGL)