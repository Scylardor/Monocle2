#include "App3D.h"

#include "Core/Exception/RuntimeException.h"
#include "GameFramework/Service/ConfigService/ConfigService.h"

#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/LogService/LogService.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/RenderService/RenderPass/GeometryRenderPass.h"
#include "GameFramework/Service/TimeService/TimeService.h"
#include "GameFramework/Service/WindowService/WindowService.h"


moe::App3D::App3D(Engine& owner, int argc, char** argv) :
	AbsSimulation(owner)
{
	EditEngine()->AddService<ConfigService>(argc, argv);

	LogService* log = EditEngine()->AddService<LogService>();
	using IDELogger = StdLogger<SeverityFilterPolicy, DebuggerFormatPolicy, IdeWritePolicy>;
	using ConsoleLogger = StdLogger<NoFilterPolicy, NoFormatPolicy, OutStreamWritePolicy>;

	IDELogger& ideLog = log->EmplaceLogger<IDELogger>();
	ideLog.GetFilterPolicy().SetFilterSeverity(moe::SevWarning);

	log->EmplaceLogger<ConsoleLogger>();

	TimeService* time = EditEngine()->AddService<TimeService>();
	time->Start();

	EditEngine()->AddService<RenderService>();
}


void moe::App3D::Start()
{
	IWindow* window = EditEngine()->EditService<WindowService>()->CreateWindow();
	if (window == nullptr)
	{
		throw RuntimeException("Could not create window");
	}
}


void moe::App3D::Update()
{
	TimeService* time = EditEngine()->EditService<TimeService>();
	time->Update();

	float dt = time->GetFrameDeltaTime();

	MOE_LOG("Delta time: %f!", dt);
}


void moe::App3D::Shutdown()
{
}

