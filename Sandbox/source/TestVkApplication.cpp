#include "TestVkApplication.h"

#include "Graphics/Vulkan/RenderScene/RenderScene.h"


#include "GameFramework/Resources/Resource/Resource.h"
#include "Graphics/Vertex/VertexFormats.h"

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

		m_manager.SetMeshFactory(m_renderer.GraphicsDevice().MeshFactory);
		m_manager.SetTextureFactory(m_renderer.GraphicsDevice().TextureFactory);

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

		m_planes =  m_manager.LoadMesh("Planes",
			sizeof(moe::BasicVertex), vertices.size(), vertices.data(),
			indices.size(), indices.data(), vk::IndexType::eUint16);
		m_scene.Emplace(m_planes.ID(), 0);

		VulkanTextureBuilder builder;
		m_statue = m_manager.Load<TextureResource>(HashString("StatueTex"), m_renderer.GraphicsDevice().TextureFactory, "Sandbox/assets/textures/texture.jpg", builder);
		Ref<TextureResource> dup = m_manager.Load<TextureResource>(HashString("StatueTex"), m_renderer.GraphicsDevice().TextureFactory, "Sandbox/assets/textures/texture.jpg", builder);

		{
			VulkanTextureBuilder builder3;
			Ref<TextureResource> block = m_manager.Load<TextureResource>(HashString("Blockc"), m_renderer.GraphicsDevice().TextureFactory, "Sandbox/assets/textures/block.png", builder3);
			(void)block;
		}

		Vec3 cameraPos{0.1f,0.5f, 10.f};
		m_view = Mat4::LookAtMatrix(cameraPos, Vec3::ZeroVector(), Vec3{ 0, 0, 1 });

		float aspectRatio = GetWindowWidth() / (float)GetWindowHeight();
		m_projection = Mat4::PerspectiveZeroOne(45_degf, aspectRatio, 0.1f, 100.f);


		//GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
		//The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix.
		//If you don't do this, then the image will be rendered upside down.
		// TODO : I think it's simpler to just use a negative viewport size in Vulkan instead.
		m_projection[1][1] *= -1;

		Mat4 model = Mat4::Rotation(Degs_f{ 45.f }, Vec3{ 0, 0, 1 });

		m_scene.MutObject(0).MutateMVP() = m_projection * m_view * model;


		AssimpImporter& assimp = m_manager.EmplaceAssetImporter<AssimpImporter>(m_renderer.GraphicsDevice());
		m_backpack = assimp.ImportModel("Sandbox/assets/objects/backpack/backpack.obj");

		model = Mat4::Identity();
		for (const MeshResource& rsc : m_backpack.GetMeshResources())
		{
			auto drawID = m_scene.Emplace(rsc.ID(), 0);

			m_scene.MutObject(drawID).MutateMVP() = m_projection * m_view * model;
		}

	}

	void BasicVkApp::Update()
	{
		TestVkApplication::Update();


		float time = GetElapsedSecondsSinceCreation();

		Mat4 model = Mat4::Rotation(Degs_f{ time * 90.f }, Vec3{ 0, 0, 1 });

		m_scene.MutObject(0).MutateMVP() = m_projection * m_view * model;
	}

}
