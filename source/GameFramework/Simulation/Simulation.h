// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include "GameFramework/Engine/EnginePart.h"


namespace moe
{
	class Engine;
	class Service;

	/*
	 * The base virtual class for Engine simulations
	 */
	class ISimulation
	{
	public:

		virtual ~ISimulation() = default;

		virtual void	Start() = 0;

		virtual void	Update() = 0;

		virtual void	Shutdown() = 0;

	};


	/*
	 * Abstract Simulation
	 */
	class AbsSimulation : public ISimulation, public EnginePart
	{
	public:

		enum class SimulationState : uint8_t
		{
			Running = 0,
			Stopping
		};

		AbsSimulation(Engine& myEngine) :
			EnginePart(myEngine)
		{}


	protected:

		SimulationState	m_runningState = SimulationState::Running;

	};

}


#include "Simulation.inl"