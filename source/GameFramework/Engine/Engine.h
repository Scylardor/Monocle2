#pragma once
#include <typeindex>

#include "Core/Containers/Vector/Vector.h"
#include "Core/Delegates/event.h"
#include "GameFramework/Simulation/Simulation.h"
#include "GameFramework/Service/Service.h"

namespace moe
{

	class Engine
	{

	public:

		using StartEvent = Event<void()>;

		Engine() = default;

		void	Start();

		StartEvent& OnStartEvent()
		{
			return m_onStart;
		}


		template <typename TSimu, typename... Args>
		TSimu*	AddSimulation(Args&&... args);

		void	RemoveSimulation(ISimulation* toRemove);

		template <typename TServ, typename... Args>
		TServ* AddService(Args&&... args);

		template <typename TServ>
		TServ const* GetService() const;

		template <typename TServ>
		TServ* EditService();


	private:

		StartEvent	m_onStart;

		Vector<std::unique_ptr<ISimulation>>	m_simulations;

		std::unordered_map<std::type_index, std::unique_ptr<Service>>	m_services;

	};

}

#include "Engine.inl"