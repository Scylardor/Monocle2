// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Literals.h" // width and height

#include "Monocle_Application_Export.h"

#include "Graphics/Renderer/Renderer.h"

namespace moe
{
	/*	Application is an abstract class meant to serve as a base for all our application types (Glfw, Sdl, Win32, etc.)
		All applications share at least three components : a window, an input handler, and a graphics context bound to the window.
	*/
	class Monocle_Application_API Application
	{
	public:
		virtual ~Application() {}

		bool	IsInitialized() const
		{
			return m_initialized;
		}


		virtual const IGraphicsRenderer&	GetRenderer() const = 0;
		virtual IGraphicsRenderer&			MutRenderer() = 0;


		[[nodiscard]] virtual Width_t	GetWindowWidth() const = 0;
		[[nodiscard]] virtual Height_t GetWindowHeight() const = 0;

	protected:

		virtual float	GetApplicationTimeSeconds() const = 0;

		bool	SetInitialized(bool init)
		{
			m_initialized = init;
			return m_initialized;
		}

		bool	m_initialized = false;
	};
}
