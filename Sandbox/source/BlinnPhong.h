// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "GameFramework/Service/RenderService/RenderScene/RenderObject.h"
#include "GameFramework/Simulation/App3D/OpenGLApp3D.h"
#include "Math/Matrix.h"
#include "GameFramework/Service/RenderService/RenderScene/RenderScene.h"

namespace moe
{
	class FlyingCameraController;
	class TimeService;

	class BlinnPhong : public OpenGLApp3D
	{
	public:

		BlinnPhong(Engine& owner, int argc, char** argv) :
			OpenGLApp3D(owner, argc, argv)
		{}

		void Start() override;

		void Update() override;

		Renderer& InitializeRenderer();

	private:

		TimeService* m_svcTime = nullptr;
		Mat4			m_quadTrans = Mat4::Identity();
		RenderScene*	m_scene = nullptr;
		FlyingCameraController* m_camCtrl = nullptr;

		RenderScene::LightID	m_pointLightID;
		RenderScene::LightID	m_DirLightID;
		RenderScene::LightID	m_SpotLightID;

		inline static const auto NUM_CUBES = 5;
		RenderObjectHandle	m_cubes[NUM_CUBES];
		RenderObjectHandle	m_floor;
	};

}
