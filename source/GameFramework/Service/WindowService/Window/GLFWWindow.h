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

		~GLFWWindow() override;


		void				PollInputs() override;

		void				SwapBuffers() override;

		std::pair<int, int>	GetDimensions() const override;

		bool				ShouldClose() override
		{
			return m_openState == WindowState::Closed;
		}


		bool			Create(ConfigSection const& windowConfig);

		virtual void	SetWindowHints(ConfigSection const& windowConfig);


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

		enum class WindowState : char
		{
			Opened,
			Closed
		};


		GLFWwindow* m_window = nullptr;

		WindowState	m_openState{ WindowState::Opened };

		static void	WindowFramebufferResizeCallback(GLFWwindow* window, int width, int height);

		static void	WindowCursorPosCallback(GLFWwindow* window, double xpos, double ypos);

		static void	WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static void	WindowMouseCallback(GLFWwindow* window, int button, int action, int mods);

		static void	WindowMouseScrollCallback(GLFWwindow* window, double xscroll, double yscroll);
	};
}


#endif