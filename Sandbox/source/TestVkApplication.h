#pragma once
#ifdef MOE_VULKAN
#include "Application/GlfwApplication/Vulkan/VulkanGlfwApplication.h"

#include "Application/GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.h"

#include "Graphics/Vulkan/RenderScene/RenderScene.h"

#include "Core/Resource/ResourceManager.h"
#include "GameFramework/Resources/AssetImporter/AssimpAssetImporter.h"


#include <chrono>

namespace moe
{


	class TestVkApplication : public moe::VulkanGlfwApplication
	{
	public:

		TestVkApplication(const moe::VulkanGlfwAppDescriptor& appDesc);
		~TestVkApplication() = default;

		void	Run();

		virtual void	Update();

		virtual void	UpdateInputs() {}

		float	GetElapsedSecondsSinceCreation() const;

	protected:

		using Clock = std::chrono::high_resolution_clock;
		using Timepoint = std::chrono::time_point<Clock>;
		using ms = std::chrono::duration<float, std::milli>;
		using sec = std::chrono::duration<float>;

		Clock		m_clock{};
		Timepoint	m_appClockStart{};
		Timepoint	m_frameClockStart{};
		float		m_deltaTime{ 0.f };


		VulkanRenderScene		m_scene;

	};


	class BasicVkApp : public TestVkApplication
	{
	public:
		BasicVkApp(const moe::VulkanGlfwAppDescriptor& appDesc);

		void Update() override;

		void	UpdateInputs() override;

	private:

		void	LoadDefaultGraphicsResources();

		void	CreateCamera();
		void	SetupCameraInputs();
		void	CameraMoveForward();
		void	CameraMoveBackwards();
		void	CameraMoveStrafeLeft();
		void	CameraMoveStrafeRight();

		void	CameraRotate(double xpos, double ypos);

		void	CreateDefaultPlanesMesh();

		void	LoadBackpackModel();

		moe::Model m_backpack;

		Ref<MeshResource> m_planes;

		Ref<TextureResource> m_statue;

		CameraRef	m_myCam;
		bool		m_moveForward = false;
		bool		m_moveBackward = false;
		bool		m_strafeLeft = false;
		bool		m_strafeRight = false;

		inline static const double UNINTIALIZED_MOUSE_COORDINATE = (float)~0;
		double		m_lastMouseX = UNINTIALIZED_MOUSE_COORDINATE;
		double		m_lastMouseY = UNINTIALIZED_MOUSE_COORDINATE;
	};
}
#endif