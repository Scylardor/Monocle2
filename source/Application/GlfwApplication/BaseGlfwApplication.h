// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLFW

#include <functional>

#include "Core/Containers/HashMap/HashMap.h"

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

		class InputManager
		{
		public:

			using KeyMapping = std::unordered_map<int, InputKeyCallback>;

			void	SetKeyBinding(int key, int action, InputKeyCallback&& callback)
			{
				auto actionMappingsIt = m_actionBindings.Find(action);
				if (actionMappingsIt == m_actionBindings.End())
				{
					std::unordered_map<int, Application::InputKeyCallback> newMap{ {key, std::move(callback) }};

					m_actionBindings.Insert({action, std::move(newMap)} );
				}
				else
				{
					// Do not use insert as it would not replace an already existing mapping; use operator[] here
					actionMappingsIt->second[key] = std::move(callback);
				}
			}


			void	SetMouseMoveBinding(InputMouseMoveCallback&& callback)
			{
				m_mouseMoveBindings.PushBack(std::move(callback));
			}


			void	SetMouseScrollBinding(InputMouseScrollCallback&& callback)
			{
				m_mouseScrollBindings.PushBack(std::move(callback));
			}


			void	CallKeyboardInputCallback(int key, int action)
			{
				auto actionMapsIt = m_actionBindings.Find(action);
				if (actionMapsIt != m_actionBindings.End())
				{
					auto callbackIt = actionMapsIt->second.find(key);
					if (callbackIt != actionMapsIt->second.end())
						callbackIt->second();
				}
			}


			void	CallMouseMoveCallbacks(double xpos, double ypos)
			{
				for (InputMouseMoveCallback& mmCallback : m_mouseMoveBindings)
				{
					mmCallback(xpos, ypos);
				}
			}


			void	CallMouseScrollCallbacks(double xoffset, double yoffset)
			{
				for (InputMouseScrollCallback& mmCallback : m_mouseScrollBindings)
				{
					mmCallback(xoffset, yoffset);
				}
			}

		private:
			HashMap<int, KeyMapping>			m_actionBindings;
			Vector<InputMouseMoveCallback>		m_mouseMoveBindings;
			Vector<InputMouseScrollCallback>	m_mouseScrollBindings;

		};

	public:
		Monocle_Application_API BaseGlfwApplication();
		Monocle_Application_API	~BaseGlfwApplication() override;

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

		void	SetInputKeyMapping(int key, int action, InputKeyCallback&& callback) override
		{
			m_inputMgr.SetKeyBinding(key, action, std::move(callback));
		}

		void	SetInputMouseMoveMapping(InputMouseMoveCallback&& callback) override
		{
			m_inputMgr.SetMouseMoveBinding(std::move(callback));
		}

		void	SetInputMouseScrollMapping(InputMouseScrollCallback&& callback) override
		{
			m_inputMgr.SetMouseScrollBinding(std::move(callback));
		}

		Monocle_Application_API	std::pair<float, float> GetMouseCursorPosition() override;

		/* Initializes the graphics context to be owned by the calling thread. */
		Monocle_Application_API void	InitializeGraphicsContext() override;

		/* Releases the graphics context owned by the calling thread so that another thread can grab it. */
		Monocle_Application_API void	ReleaseGraphicsContext() override;

		static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
		static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
		static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


	private:

		/**
		 * \brief The handle to our current window. Must be set with a call to CreateGlfwWindow.
		 */
		GLFWwindow* m_window = nullptr;

		AppDescriptor	m_description;

		InputManager	m_inputMgr;
	};
}

#endif // MOE_GLFW