// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Literals.h" // width and height

#include "Monocle_Application_Export.h"

#include "Graphics/Renderer/Renderer.h"

#include <utility> // std::pair

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

		using InputKeyCallback = std::function<void()>;
		using InputMouseMoveCallback = std::function<void (double, double)>;
		using InputMouseScrollCallback = std::function<void(double, double)>;


		virtual float	GetApplicationTimeSeconds() const = 0;

		virtual void	SetInputKeyMapping(int key, int action, InputKeyCallback&& callback) = 0;
		virtual void	SetInputMouseMoveMapping(InputMouseMoveCallback&& callback) = 0;
		virtual void	SetInputMouseScrollMapping(InputMouseScrollCallback&& callback) = 0;


		virtual std::pair<float, float>	GetMouseCursorPosition() = 0;

		bool	SetInitialized(bool init)
		{
			m_initialized = init;
			return m_initialized;
		}

		bool	m_initialized = false;
	};
}
