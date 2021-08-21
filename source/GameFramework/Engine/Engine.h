#pragma once
#include "GameFramework/Simulation/Simulation.h"

namespace moe
{
	class Engine
	{

	public:

		Engine() = default;

		template <typename TSimu, typename... Args>
		TSimu*	AddNewSimulation(Args&&... args);

		void	RemoveSimulation(ISimulation* toRemove)
		{
			auto simuIt = std::find_if(m_simulations.begin(), m_simulations.end(), [toRemove](const auto& simuPtr)
			{
				return simuPtr.get() == toRemove;
			});

			if (simuIt != m_simulations.end())
				m_simulations.erase(simuIt);
		}

	private:

		std::vector<std::unique_ptr<ISimulation>>	m_simulations;
	};

}

#include "Engine.inl"