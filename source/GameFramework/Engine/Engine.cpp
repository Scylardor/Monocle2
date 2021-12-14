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


	void Engine::RemoveShuttingDownSimulations()
	{
		// Sort indices in descending order so simulations that want to be destroyed don't end up swapped before!
		std::sort(m_simulationsShuttingDown.Begin(), m_simulationsShuttingDown.End(), std::greater<>());
		for (auto idx : m_simulationsShuttingDown)
		{
			m_simulations.EraseBySwapAt(idx);
		}

		m_simulationsShuttingDown.Clear();

		if (m_simulations.Empty())
		{
			m_state = State::Stopped; // No more simulations to run ? The engine goes down.
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

			// Shut down all simulations that noticed us they wanna get destroyed.
			if (!m_simulationsShuttingDown.Empty())
			{
				RemoveShuttingDownSimulations();
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
		{
			m_simulationsShuttingDown.PushBack((uint32_t) std::distance(m_simulations.Begin(), simuIt));
		}
	}
}