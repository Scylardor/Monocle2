// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "GameFramework/Service/RenderService/RenderScene/RenderObject.h"
#include "GameFramework/Simulation/App3D/OpenGLApp3D.h"
#include "Math/Matrix.h"

namespace moe
{
	class TimeService;

	class BasicQuad : public OpenGLApp3D
	{
	public:

		BasicQuad(Engine& owner, int argc, char** argv) :
			OpenGLApp3D(owner, argc, argv)
		{}

		void Start() override;

		void Update() override;

		Renderer& InitializeRenderer();

	private:

		TimeService*	m_svcTime = nullptr;
		Mat4			m_quadTrans = Mat4::Identity();
		RenderScene*	m_scene = nullptr;
		RenderObjectHandle	m_quad;
	};

}
