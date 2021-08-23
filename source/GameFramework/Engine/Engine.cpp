#include "Engine.h"


namespace moe
{
	void Engine::Run()
	{
		m_onStart.Broadcast();
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