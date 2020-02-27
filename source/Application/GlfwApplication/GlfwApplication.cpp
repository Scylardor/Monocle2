// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_GLFW

#include "GlfwApplication.h"

#include <GLFW/glfw3.h>

#include "Window/WindowDescriptor.h"
#include "Window/GlfwWindow/GlfwWindow.h"

#include "Graphics/Context/GlfwGraphicsContext/GlfwGraphicsContext.h"

#include "Input/GlfwInputHandler/GlfwInputHandler.h"

moe::GlfwApplication::GlfwApplication(const GraphicsContextDescriptor& contextDesc, const WindowDescriptor& windowDesc)
{
	if (glfwInit())
	{
		InitContext<GlfwGraphicsContext>();
		m_graphicsContext->InitPreWindowCreation(contextDesc);

		// precise moe::GlfwWindow to not confuse with GLFWwindow...
		InitWindow<moe::GlfwWindow>(windowDesc);

		m_initialized = m_graphicsContext->InitPostWindowCreation(contextDesc, m_window.get());

		InitInput<moe::GlfwInputHandler>();

		m_initialized &= (m_graphicsContext != nullptr && m_inputHandler != nullptr && m_window != nullptr);
	}
}


moe::GlfwApplication::~GlfwApplication()
{
	glfwTerminate();
}

#endif // MOE_GLFW