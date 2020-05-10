// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLFW

#include "Monocle_Application_Export.h"

#include "Application/Application.h"

#include "Application/AppDescriptor/AppDescriptor.h"


struct GLFWwindow;

namespace moe
{
	/**
	 * \brief Base class used for all generic GLFW operations (window creation, etc.).
	 * Specific graphics API logic (OpenGL, Vulkan) is to be done in child classes.
	*/
	class BaseGlfwApplication : public Application
	{
	public:
		Monocle_Application_API BaseGlfwApplication();
		Monocle_Application_API virtual ~BaseGlfwApplication();

	protected:


		/**
		 * \brief Creates a GLFW window using parameters set in the app description and sets \ref m_window with it.
		 * Note : this function doesn't get automatically called by the constructor.
		 * Child classes must call it explicitly in order to have a chance to set more glfwWindowHints if they want to.
		 * \param appDesc : the description to use to create the window (width, height, title etc.)
		 * \return The created window or nullptr if the window creation has failed.
		 */
		GLFWwindow*	CreateGlfwWindow(const AppDescriptor& appDesc);


		GLFWwindow*	GetGlfwWindow()
		{
			MOE_DEBUG_ASSERT(m_window != nullptr);
			return m_window;
		}


		Monocle_Application_API void	PollInputEvents();

		Monocle_Application_API void	SwapBuffers();

		Monocle_Application_API bool	WindowIsOpened() const;

		Monocle_Application_API float	GetApplicationTimeSeconds() const override;

	private:

		/**
		 * \brief The handle to our current window. Must be set with a call to CreateGlfwWindow.
		 */
		GLFWwindow* m_window = nullptr;

		AppDescriptor	m_description;
	};
}

#endif // MOE_GLFW