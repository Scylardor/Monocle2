// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLFW

#include "Core/Preprocessor/moeDLLVisibility.h"

#include "Window/Window.h"

#include "Window/WindowDescriptor.h"

class GLFWwindow;

namespace moe
{

	class MOE_DLL_API GlfwWindow : public IWindow
	{
	public:
		GlfwWindow(const WindowDescriptor& desc);
		~GlfwWindow() = default;

	private:
		GLFWwindow* m_window = nullptr;
	};

}


#endif // MOE_GLFW