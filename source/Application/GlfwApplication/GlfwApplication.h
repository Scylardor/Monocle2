// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLFW

#include "Core/Preprocessor/moeDLLVisibility.h"

#include "Application/Application.h"

#include "Window/WindowDescriptor.h"

#include "Graphics/Context/GraphicsContextDescriptor.h"

namespace moe
{

	/*	Application is an abstract class meant to serve as a base for all our application types (Glfw, Sdl, Win32, etc.)
		All applications share at least three components : a window, an input handler, and a graphics context bound to the window.
	*/
	class MOE_DLL_API GlfwApplication : public Application
	{
	public:
		GlfwApplication(const GraphicsContextDescriptor& contextDesc, const WindowDescriptor& windowDesc);
		~GlfwApplication();
	};
}

#endif // MOE_GLFW