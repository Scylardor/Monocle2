// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include "GameFramework/Simulation/Simulation.h"

namespace moe
{
	class App3D : public AbstractSimulation
	{
	public:

		App3D(int argc, char** argv);

		void Start() override;

		void Update() override;

		void Finish() override;

	private:


	};


}
