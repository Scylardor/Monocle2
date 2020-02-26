// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Input/InputHandler.h"
#include "Window/Window.h"
#include <memory> // unique ptr

namespace moe
{
	/*	Application is an abstract class meant to serve as a base for all our application types (Glfw, Sdl, Win32, etc.)
		All applications share at least three components : a window, an input handler, and a graphics context bound to the window.
	*/
	class Application
	{
		
	protected:

		std::unique_ptr<InputHandler>	m_inputHandler;
		std::unique_ptr<IWindow>		m_window;
	};
}