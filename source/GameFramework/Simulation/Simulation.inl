#pragma once

#include <type_traits>

namespace moe
{

	template <typename TServ, typename ... Args>
	TServ* AbstractSimulation::Add(Args&&... args)
	{
		static_assert(std::is_base_of_v<Service, TServ>, "not a Service derived type");

		auto [it, _] = m_services.try_emplace(typeid(TServ), std::make_unique<TServ>(std::forward<Args>(args)...));

		return  static_cast<TServ*>(it->second.get());
	}


	template <typename TServ>
	TServ const* AbstractSimulation::Get() const
	{
		auto foundIt = m_services.find(typeid(TServ));
		if (foundIt == m_services.end())
			return nullptr;

		return static_cast<TServ const*>(foundIt->second.get());
	}


	template <typename TServ>
	TServ* AbstractSimulation::Edit()
	{
		auto foundIt = m_services.find(typeid(TServ));
		if (foundIt == m_services.end())
			return nullptr;

		return static_cast<TServ*>(foundIt->second.get());
	}
}
