
#ifdef MOE_GLFW

#include "GlfwWindow.h"

#include "Window/WindowDescriptor.h"

#include "Core/Preprocessor/moeAssert.h"

#include <GLFW/glfw3.h>

moe::GlfwWindow::GlfwWindow(const WindowDescriptor& desc)
{
	m_window = glfwCreateWindow(desc.m_width, desc.m_height, desc.m_title, nullptr, nullptr);
	MOE_DEBUG_ASSERT(m_window != nullptr);
}


moe::GlfwWindow::~GlfwWindow()
{
	glfwDestroyWindow(m_window);
}


bool moe::GlfwWindow::ShouldWindowClose() const
{
	return (glfwWindowShouldClose(m_window));
}

#endif // MOE_GLFW