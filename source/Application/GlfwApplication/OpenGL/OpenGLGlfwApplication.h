// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLFW

#include "Core/Preprocessor/moeDLLVisibility.h"

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
	};
}

#endif // MOE_GLFW