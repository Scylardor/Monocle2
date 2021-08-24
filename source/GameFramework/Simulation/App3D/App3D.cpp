#include "App3D.h"

#include "GameFramework/Service/ConfigService/ConfigService.h"

#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/LogService/LogService.h"


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
}

