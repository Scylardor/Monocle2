// Monocle Game Engine source files - Alexandre Baron

#if defined(MOE_GLFW ) && defined(MOE_OPENGL)

#include "OpenGLGLFWService.h"

#endif
#include "GameFramework/Service/WindowService/Window/GLFWWindow.h"

moe::OpenGLGLFWService::OpenGLGLFWService(Engine& engine) :
	GLFWService(engine)
{
}


std::unique_ptr<moe::IWindow> moe::OpenGLGLFWService::CreateWindow()
{
	return std::make_unique<OpenGLGLFWWindow>(*this);
}
