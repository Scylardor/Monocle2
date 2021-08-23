#include "App3D.h"

#include "GameFramework/Service/ConfigService/ConfigService.h"

#include "GameFramework/Engine/Engine.h"


moe::App3D::App3D(Engine& owner, int argc, char** argv) :
	AbsSimulation(owner)
{
	moe::GetLogChainSingleton().LinkTo(&m_IDELogger);
	m_IDELogger.GetFilterPolicy().SetFilterSeverity(moe::SevWarning);
	m_consoleLogger.LinkTo(&m_IDELogger);

	EditEngine()->AddService<ConfigService>(argc, argv);
}

