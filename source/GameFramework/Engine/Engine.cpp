#include "Engine.h"


namespace moe
{
	void Engine::Start()
	{
		m_onStart.Broadcast();

		for (auto& simu : m_simulations)
		{
			simu->Start();
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