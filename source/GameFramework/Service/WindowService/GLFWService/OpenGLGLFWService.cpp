// Monocle Game Engine source files - Alexandre Baron

#if defined(MOE_GLFW ) && defined(MOE_OPENGL)

#include "GameFramework/Engine/Engine.h"
#include "OpenGLGLFWService.h"

#include "GameFramework/Service/ConfigService/ConfigService.h"
#include "GameFramework/Service/WindowService/Window/OpenGLGLFWWindow.h"

moe::OpenGLGLFWService::OpenGLGLFWService(Engine& engine) :
	GLFWService(engine)
{
	auto const* Config = EditEngine()->GetService<ConfigService>();
	MOE_ASSERT(Config != nullptr);

	auto const windowSection = Config->GetSection("window");

	auto* window = EmplaceWindow<OpenGLGLFWWindow>();

	window->SetWindowHints(windowSection);
	bool ok = window->Create(windowSection);
	MOE_DEBUG_ASSERT(ok);
}


moe::IWindow* moe::OpenGLGLFWService::CreateWindow()
{
	return nullptr;
}
#endif
