// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Input/InputHandler.h"
#include "Window/Window.h"
#include "Graphics/Context/GraphicsContext.h"
#include <memory> // unique ptr

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
			return m_graphicsContext != nullptr && m_inputHandler != nullptr && m_window != nullptr;
		}

	protected:

		template <typename TWindow, typename... Args>
		void	InitWindow(Args&&... args)
		{
			static_assert(std::is_base_of<IWindow, TWindow>::value, "Window must inherit from IWindow");
			m_window = std::make_unique<TWindow>(std::forward<Args>(args)...);
			MOE_DEBUG_ASSERT(m_window != nullptr);
		}

		template <typename TInput, typename... Args>
		void	InitInput(Args&&... args)
		{
			static_assert(std::is_base_of<InputHandler, TInput>::value, "Input must inherit from InputHandler");
			m_inputHandler = std::make_unique<TInput>(std::forward<Args>(args)...);
			MOE_DEBUG_ASSERT(m_inputHandler != nullptr);
		}

		template <typename TContext, typename... Args>
		void	InitContext(Args&&... args)
		{
			static_assert(std::is_base_of<IGraphicsContext, TContext>::value, "Context must inherit from IGraphicsContext");
			m_graphicsContext = std::make_unique<TContext>(std::forward<Args>(args)...);
			MOE_DEBUG_ASSERT(m_graphicsContext != nullptr);
		}

		std::unique_ptr<IGraphicsContext>	m_graphicsContext;
		std::unique_ptr<InputHandler>		m_inputHandler;
		std::unique_ptr<IWindow>			m_window;
	};
}