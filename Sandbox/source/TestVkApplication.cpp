#include "TestVkApplication.h"


#include <chrono>

namespace moe
{



	TestVkApplication::TestVkApplication(const moe::VulkanGlfwAppDescriptor& appDesc) :
		VulkanGlfwApplication(appDesc)
	{
		m_appClockStart = m_clock.now();
		m_frameClockStart = m_appClockStart;
	}

	void TestVkApplication::Run()
	{




		while (WindowIsOpened())
		{
			PollInputEvents();

			Update();

			m_renderer.RenderFrame(m_scene);
		}

		m_renderer.Shutdown();

	}


	void TestVkApplication::Update()
	{
		auto stop = m_clock.now();
		//auto dtms = std::chrono::duration_cast<ms>(stop - m_clockStart).count();
		m_deltaTime = std::chrono::duration_cast<sec>(stop - m_frameClockStart).count();
		m_frameClockStart = stop;
		//std::cout << "dt milli : " << dtms << std::endl;
		//std::cout << "dt sec : " << m_deltaTime << std::endl;

	}


	float TestVkApplication::GetElapsedSecondsSinceCreation() const
	{
		return std::chrono::duration_cast<sec>(m_frameClockStart - m_appClockStart).count();
	}


	BasicVkApp::BasicVkApp(const moe::VulkanGlfwAppDescriptor& appDesc) :
		TestVkApplication(appDesc)
	{

		const std::vector<moe::BasicVertex> vertices =
		{
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices =
		{
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		VkDeviceSize vertexSize = sizeof(vertices[0]) * vertices.size();
		auto meshID = m_renderer.EmplaceMesh(vertexSize, vertices.size(), vertices.data(), indices.size(), indices.data(), vk::IndexType::eUint16);

		m_scene.Emplace(meshID, 0);

		Vec3 cameraPos{0.1f,0.5f, 1.f};
		m_view = Mat4::LookAtMatrix(cameraPos, Vec3::ZeroVector(), Vec3{ 0, 0, 1 });

		float aspectRatio = GetWindowWidth() / (float)GetWindowHeight();
		m_projection = Mat4::PerspectiveZeroOne(45_degf, aspectRatio, 0.1f, 10.f);


		//GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
		//The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix.
		//If you don't do this, then the image will be rendered upside down.
		// TODO : I think it's simpler to just use a negative viewport size in Vulkan instead.
		m_projection[1][1] *= -1;

		Mat4 model = Mat4::Identity();// Rotation(Degs_f{ 45.f }, Vec3{ 0, 0, 1 });

		m_scene.MutObject(0).MutateMVP() = m_projection * m_view * model;

		m_manager.SetMeshFactory(m_renderer.GraphicsDevice().MeshFactory);

		AssimpImporter& assimp = m_manager.EmplaceAssetImporter<AssimpImporter>();
		m_backpack = assimp.ImportModel("Sandbox/assets/objects/backpack/backpack.obj");

		for (const moe::MeshResource& rsc : m_backpack.GetMeshResources())
		{
			m_scene.Emplace(rsc.ID(), 0);
		}
	}

	void BasicVkApp::Update()
	{
		TestVkApplication::Update();


		//float time = GetElapsedSecondsSinceCreation();
		//
		//Mat4 model = Mat4::Rotation(Degs_f{ time * 90.f }, Vec3{ 0, 0, 1 });
		//
		//m_scene.MutObject(0).MutateMVP() = m_projection * m_view * model;
	}

}
