#include "OpenGLApp3D.h"



#include "Core/Exception/RuntimeException.h"
#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/WindowService/GLFWService/OpenGLGLFWService.h"


moe::OpenGLApp3D::OpenGLApp3D(Engine& owner, int argc, char** argv) :
	moe::App3D(owner, argc, argv)
{
	EditEngine()->AddService<OpenGLGLFWService>();

	m_startHandle = EditEngine()->OnStartEvent().Add<moe::OpenGLApp3D, &moe::OpenGLApp3D::Start>(this);
}


moe::OpenGLApp3D::~OpenGLApp3D()
{
	EditEngine()->OnStartEvent().Remove(m_startHandle);
}


void moe::OpenGLApp3D::Start()
{
	IWindow* window = EditEngine()->EditService<WindowService>()->CreateWindow();
	if (window == nullptr)
	{
		throw RuntimeException("Could not create window");
	}
}
