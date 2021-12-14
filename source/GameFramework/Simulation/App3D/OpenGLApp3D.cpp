#include "OpenGLApp3D.h"


#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/WindowService/GLFWService/OpenGLGLFWService.h"
#include "Graphics/RHI/OpenGL/OGL4RHI.h"


moe::OpenGLApp3D::OpenGLApp3D(Engine& owner, int argc, char** argv) :
	moe::App3D(owner, argc, argv)
{
}


void moe::OpenGLApp3D::Start()
{
	EditEngine()->AddService<OpenGLGLFWService>();

	App3D::Start();

	auto* rdrSvc = EditEngine()->EditService<RenderService>();
	rdrSvc->EmplaceRenderHardwareInterface<OpenGL4RHI>(OpenGLGLFWService::GetGLLoaderFunction());
}
