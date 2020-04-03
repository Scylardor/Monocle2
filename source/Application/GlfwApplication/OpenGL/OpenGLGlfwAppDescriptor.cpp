// Monocle Game Engine source files - Alexandre Baron

#pragma once

#if defined(MOE_GLFW) && defined(MOE_OPENGL)

#include "OpenGLGlfwAppDescriptor.h"

#include <GLFW/glfw3.h> // GLFW_OPENGL_CORE_PROFILE

namespace moe
{
	OpenGLGlfwAppDescriptor::OpenGLGlfwAppDescriptor(Width_t w, Height_t h, const char* windowTitle, const char* iconPath, bool resizableWin) :
	AppDescriptor(w, h, windowTitle, iconPath, resizableWin), m_openGLProfile(GLFW_OPENGL_CORE_PROFILE)
	{}
}

#endif // defined(MOE_GLFW) && defined(MOE_OPENGL)