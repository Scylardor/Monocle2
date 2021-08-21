// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include <memory> // unique ptr
#include <typeindex>
#include <unordered_map>

#include "GameFramework/Service/Service.h"

namespace moe
{
	/*
	 * The base virtual class for Engine simulations
	 */
	class ISimulation
	{
	public:

		virtual ~ISimulation() = default;

		virtual void	Start() = 0;

		virtual void	Update() = 0;

		virtual void	Finish() = 0;
	};



	class AbstractSimulation : public ISimulation
	{
	public:

		template <typename TServ, typename... Args>
		TServ* Add(Args&&... args);

		template <typename TServ>
		TServ const* Get() const;

		template <typename TServ>
		TServ* Edit();

	private:

		std::unordered_map<std::type_index, std::unique_ptr<Service>>	m_services;

	};

}


#include "Simulation.inl"