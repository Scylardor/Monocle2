// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include "GameFramework/Simulation/Simulation.h"

namespace moe
{
	class App3D : public AbsSimulation
	{
	public:

		App3D(Engine& owner, int argc, char** argv);

	private:
		using IDELogger = moe::StdLogger<moe::SeverityFilterPolicy, moe::DebuggerFormatPolicy, moe::IdeWritePolicy>;
		using ConsoleLogger = moe::StdLogger<moe::NoFilterPolicy, moe::NoFormatPolicy, moe::OutStreamWritePolicy>;

		IDELogger		m_IDELogger;
		ConsoleLogger	m_consoleLogger;

	};


}
