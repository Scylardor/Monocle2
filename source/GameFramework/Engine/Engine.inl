#pragma once

namespace moe
{
	template <typename TSimu, typename ... Args>
	TSimu* Engine::AddNewSimulation(Args&&... args)
	{
		static_assert(std::is_base_of_v<ISimulation, TSimu>, "Not a simulation");
		m_simulations.emplace_back(std::make_unique<TSimu>(std::forward<Args>(args)...));
		return *m_simulations.back();
	}
}
