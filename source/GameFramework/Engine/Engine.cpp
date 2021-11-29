#include "Engine.h"


namespace moe
{
	void Engine::Start()
	{
		MOE_ASSERT(m_state == State::Stopped);

		m_onStart.Broadcast();

		for (auto& simu : m_simulations)
		{
			simu->Start();
		}

	}


	void Engine::Stop()
	{
		for (auto& simu : m_simulations)
		{
			simu->Shutdown();
		}
	}


	void Engine::Run()
	{
		Start();

		m_state = State::Running;

		while (m_state == State::Running)
		{
			for (auto& simu : m_simulations)
			{
				simu->Update();
			}
		}
	}


	void	Engine::RemoveSimulation(ISimulation* toRemove)
	{
		auto simuIt = std::find_if(m_simulations.begin(), m_simulations.end(), [toRemove](const auto& simuPtr)
			{
				return simuPtr.get() == toRemove;
			});

		if (simuIt != m_simulations.end())
			m_simulations.EraseBySwap(simuIt);
	}
}