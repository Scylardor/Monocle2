// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "GameFramework/Simulation/App3D/OpenGLApp3D.h"

namespace moe
{

	class BasicQuad : public OpenGLApp3D
	{
	public:

		BasicQuad(Engine& owner, int argc, char** argv) :
			OpenGLApp3D(owner, argc, argv)
		{}

		void Start() override;

		Renderer& InitializeRenderer();
	};

}
