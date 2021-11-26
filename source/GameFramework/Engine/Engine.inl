#pragma once

namespace moe
{
	template <typename TSimu, typename ... Args>
	TSimu* Engine::	AddSimulation(Args&&... args)
	{
		static_assert(std::is_base_of_v<ISimulation, TSimu>, "Not a simulation");
		m_simulations.EmplaceBack(std::make_unique<TSimu>(*this, std::forward<Args>(args)...));
		return static_cast<TSimu*>(m_simulations.Back().get());
	}


	template <typename TServ, typename ... Args>
	TServ* Engine::AddService(Args&&... args)
	{
		static_assert(std::is_base_of_v<Service, TServ>, "not a Service derived type");

		auto [it, inserted] = m_services.try_emplace(typeid(TServ::ServiceLocatorType), std::make_unique<TServ>(*this, std::forward<Args>(args)...));

		if (!inserted)
		{
			MOE_LOG("Tried to add a new service, but already had a service of the same type. Skipping");
		}

		return static_cast<TServ*>(it->second.get());
	}


	template <typename TServ>
	TServ const* Engine::GetService() const
	{
		auto foundIt = m_services.find(typeid(TServ));
		if (foundIt == m_services.end())
			return nullptr;

		return static_cast<TServ const*>(foundIt->second.get());
	}


	template <typename TServ>
	TServ* Engine::EditService()
	{
		auto foundIt = m_services.find(typeid(TServ));
		if (foundIt == m_services.end())
			return nullptr;

		return static_cast<TServ*>(foundIt->second.get());
	}
}
