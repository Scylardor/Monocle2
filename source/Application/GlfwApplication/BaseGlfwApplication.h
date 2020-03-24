// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLFW

#include "Core/Preprocessor/moeDLLVisibility.h"

#include "Application/Application.h"

#include <GLFW/glfw3.h> // GLFWwindow

namespace moe
{
	/**
	 * \brief Base class used for all generic GLFW operations (window creation, etc.).
	 * Specific graphics API logic (OpenGL, Vulkan) is to be done in child classes.
	*/
	class MOE_DLL_API BaseGlfwApplication : public Application
	{
	public:
		BaseGlfwApplication();
		virtual ~BaseGlfwApplication();

	protected:


		/**
		 * \brief Creates a GLFW window using parameters set in the app description and sets \ref m_window with it.
		 * Note : this function doesn't get automatically called by the constructor.
		 * Child classes must call it explicitly in order to have a chance to set more glfwWindowHints if they want to.
		 * \param appDesc : the description to use to create the window (width, height, title etc.)
		 * \return The created window or nullptr if the window creation has failed.
		 */
		GLFWwindow*	CreateGlfwWindow(const struct AppDescriptor& appDesc);


		GLFWwindow*	GetGlfwWindow()
		{
			MOE_DEBUG_ASSERT(m_window != nullptr);
			return m_window;
		}


	private:

		/**
		 * \brief The handle to our current window. Must be set with a call to CreateGlfwWindow.
		 */
		GLFWwindow* m_window = nullptr;
	};
}

#endif // MOE_GLFW