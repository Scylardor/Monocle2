// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include <Core/Delegates/EventDelegateID.h>

#include "App3D.h"
#include "GameFramework/Simulation/Simulation.h"

namespace moe
{
	class OpenGLApp3D : public App3D
	{
	public:

		OpenGLApp3D(Engine& owner, int argc, char** argv);
		~OpenGLApp3D();

		void Start() ;

	private:

		EventDelegateID	m_startHandle;
	};


}
