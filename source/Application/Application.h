// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Input/InputHandler/InputHandler.h"
#include <memory> // unique ptr

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


	protected:

		template <typename TInput, typename... Args>
		void	InitInput(Args&&... args)
		{
			static_assert(std::is_base_of<InputHandler, TInput>::value, "Input must inherit from InputHandler");
			m_inputHandler = std::make_unique<TInput>(std::forward<Args>(args)...);
			MOE_DEBUG_ASSERT(m_inputHandler != nullptr);
		}


		bool	SetInitialized(bool init)
		{
			m_initialized = init;
			return m_initialized;
		}


		std::unique_ptr<InputHandler>		m_inputHandler{ nullptr };
		std::unique_ptr<IGraphicsRenderer>	m_renderer{ nullptr };

		bool	m_initialized = false;
	};
}
