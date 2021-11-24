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

	};


	/*
	 * Abstract Simulation
	 */
	class AbsSimulation : public ISimulation, public EnginePart
	{
	public:

		AbsSimulation(Engine& myEngine) :
			EnginePart(myEngine)
		{}

	};

}


#include "Simulation.inl"