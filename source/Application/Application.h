// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"

#include "Graphics/Renderer/Renderer.h"

namespace moe
{
	/*	Application is an abstract class meant to serve as a base for all our application types (Glfw, Sdl, Win32, etc.)
		All applications share at least three components : a window, an input handler, and a graphics context bound to the window.
	*/
	class MOE_DLL_API Application
	{
	public:
		virtual ~Application() {}

		bool	IsInitialized() const
		{
			return m_initialized;
		}


		virtual const IGraphicsRenderer&	GetRenderer() const = 0;
		virtual IGraphicsRenderer&			MutRenderer() = 0;


	protected:


		bool	SetInitialized(bool init)
		{
			m_initialized = init;
			return m_initialized;
		}

		bool	m_initialized = false;
	};
}
