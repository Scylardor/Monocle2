#include "TestVkApplication.h"

#include "Graphics/Vulkan/RenderScene/RenderScene.h"

#include "Graphics/Vertex/VertexFormats.h"

#include "Graphics/Vulkan/Camera/CameraSystem.h"

#include <chrono>


#include <vector>
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
		LoadDefaultGraphicsResources();

		CreateCamera();

		CreateDefaultPlanesMesh();

		LoadBackpackModel();
	}

	void BasicVkApp::Update()
	{
		TestVkApplication::Update();

		float time = GetElapsedSecondsSinceCreation();

		Mat4 model = Mat4::Rotation(Degs_f{ time * 90.f }, Vec3{ 0, 0, 1 });

		m_scene.MutObject(0).MutateMVP() = m_myCam.GetViewProjection() * model;
	}

	void BasicVkApp::LoadDefaultGraphicsResources()
	{
		m_scene.Initialize(m_renderer.GraphicsDevice(), m_renderer.GetMaxFramesInFlight());

		auto textureFileLoaderFn = [this](std::string_view filename)
		{
			return [&]()
			{
				VulkanTextureBuilder builder;
				return m_renderer.GraphicsDevice().TextureFactory.CreateTextureFromFile(filename, builder);
			};
		};

		// Need to load the statue tex before creating the main material (yup, will have to change that later!)
		m_statue = m_manager.Load<TextureResource>(HashString("StatueTex"), textureFileLoaderFn("Sandbox/assets/textures/texture.jpg"));

		m_renderer.AttachResourceManager(m_manager);

		m_renderer.CreateMainMaterial();
	}

	void BasicVkApp::CreateCamera()
	{
		const float aspectRatio = GetWindowWidth() / (float)GetWindowHeight();
		m_myCam = m_scene.CameraSystem().New<PerspectiveCamera>(Vec3{ 0.1f, 0.5f, 10.f }, 45_degf, aspectRatio, 0.1f, 100.f, CameraProjection::Perspective_ZeroToOne_FlippedY);
		m_myCam.SetUpVector({ 0, 0, 1 });
		m_myCam.Lookat(Vec3::ZeroVector());
	}

	void BasicVkApp::CreateDefaultPlanesMesh()
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

		m_planes = m_manager.Load<MeshResource>(HashString("Planes"),
			[&]()
			{
				return m_renderer.GraphicsDevice().MeshFactory.NewMesh(sizeof(moe::BasicVertex), vertices.size(), vertices.data(),
					indices.size(), indices.data(), VertexIndexType::eUint16);
			});

		m_scene.Emplace(m_planes.ID(), 0);

		Mat4 model = Mat4::Rotation(Degs_f{ 45.f }, Vec3{ 0, 0, 1 });

		m_scene.MutObject(0).MutateMVP() = m_myCam.GetViewProjection() * model;
	}

	void BasicVkApp::LoadBackpackModel()
	{
		auto& assimp = m_manager.EmplaceAssetImporter<AssimpImporter>(m_renderer.GraphicsDevice());
		m_backpack = assimp.ImportModel("Sandbox/assets/objects/backpack/backpack.obj");

		Mat4 model = Mat4::Identity();

		// In case we need it
		Ref<MaterialResource> defaultMat = m_manager.FindExisting<MaterialResource>(HashString("DefaultMaterial"));
		MOE_ASSERT(defaultMat);

		for (const auto& node : m_backpack.GetNodes())
		{
			for (size_t meshIdx : node.Meshes)
			{
				const MeshData& meshData = m_backpack.GetMeshes()[meshIdx];

				RegistryID matID;
				if (meshData.Material != 0) // 0 is "no material" for assimp so use the default one
				{
					const auto& meshMaterial = m_backpack.GetMaterialResources()[meshData.Material];
					matID = meshMaterial.MaterialResource.ID();
				}
				else
				{
					matID = defaultMat.ID();
				}

				auto meshID = m_backpack.GetMeshResources()[meshIdx].ID();

				auto drawID = m_scene.Emplace(meshID, matID);

				m_scene.MutObject(drawID).MutateMVP() = m_myCam.GetViewProjection() * model;
			}
		}
	}

}
