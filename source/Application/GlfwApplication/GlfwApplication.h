// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Application/Application.h"
#include "Window/WindowDescriptor.h"

namespace moe
{
	/*	Application is an abstract class meant to serve as a base for all our application types (Glfw, Sdl, Win32, etc.)
		All applications share at least three components : a window, an input handler, and a graphics context bound to the window.
	*/
	class GlfwApplication : public Application
	{
	public:
		GlfwApplication(const WindowDescriptor& windowDesc);
		~GlfwApplication();
	};
}