// Monocle Game Engine source files - Alexandre Baron

#pragma once
#if defined(MOE_GLFW )
#include "GameFramework/Service/WindowService/GLFWService/GLFWService.h"

namespace moe
{
	class ConfigSection;
	class GLFWService;

	class GLFWWindow : public IWindow
	{
	public:

		GLFWWindow() = default;

		virtual ~GLFWWindow();

		virtual void	SetWindowHints(ConfigSection const& windowConfig);

		bool	Create(ConfigSection const& windowConfig);


	protected:

		[[nodiscard]] bool	HasWindow() const
		{
			return m_window != nullptr;
		}

		GLFWwindow*	EditWindow()
		{
			return m_window;
		}


		virtual bool	PostCreate(ConfigSection const& windowConfig);

	private:

		GLFWwindow* m_window = nullptr;
	};

}


#endif