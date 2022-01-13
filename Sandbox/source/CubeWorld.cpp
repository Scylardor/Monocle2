// Monocle Game Engine source files - Alexandre Baron

#include "CubeWorld.h"


#include "Core/Containers/Array/Array.h"
#include "Core/Resource/FileResource.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Core/Resource/MeshResource.h"
#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/InputService/InputService.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/RenderService/RenderPass/GeometryRenderPass.h"
#include "GameFramework/Service/RenderService/RenderPass/PresentPass.h"
#include "GameFramework/Service/ResourceService/ResourceService.h"
#include "GameFramework/Service/WindowService/WindowService.h"
#include "Graphics/Sampler/SamplerDescriptor.h"
#include "Graphics/ShaderCapabilities/ShaderCapabilities.h"
#include "Graphics/Vertex/VertexFormats.h"
#include "GameFramework/Service/TimeService/TimeService.h"
#include "Graphics/Geometry/Cube.h"

namespace moe
{

	void CubeWorld::Start()
	{
		OpenGLApp3D::Start();

		auto* rscSvc = EditEngine()->AddService<ResourceService>();

		auto cubeGeometry = CreateCube_PositionTexture(0.5f);

		auto cubeMesh = rscSvc->EmplaceResource<MeshResource>(
			HashString("CubeMesh"),
			MeshData{ cubeGeometry.Data(), sizeof(cubeGeometry[0]), cubeGeometry.Size() });

		auto basicVertShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BasicShader.vert"), "source/Graphics/Resources/shaders/OpenGL/basic_textured_nocolor.vert", FileMode::Text);
		auto basicFragShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BasicShader.frag"), "source/Graphics/Resources/shaders/OpenGL/basic_textured_nocolor.frag", FileMode::Text);

		MaterialDescription matDesc;
		auto& passDesc = matDesc.NewPassDescription();
		passDesc.
			AssignPipelineVertexLayout({ {
				{"position", VertexBindingFormat::Float3 },
				{"uv0", VertexBindingFormat::Float2 } }
			}).AssignShaderProgramDescription({
			{	ShaderStage::Vertex, basicVertShaderFile},
			{	ShaderStage::Fragment, basicFragShaderFile}
			})
		.Pipeline.RasterizerStateDesc.m_cullMode = CullFace::Back;
		passDesc.AddShaderCapability<SC_ObjectTransform>();

		Renderer& forwardRenderer = InitializeRenderer();

		Ref<TextureResource> containerTexture = rscSvc->EmplaceResource<TextureResource>(HashString("Container"), "Sandbox/assets/container.jpg");
		DeviceTextureHandle texHandle = forwardRenderer.MutRHI()->TextureManager().CreateTexture2DFromFile(containerTexture);

		Ref<TextureResource> awesomeTexture = rscSvc->EmplaceResource<TextureResource>(HashString("AwesomeFace"), "Sandbox/assets/awesomeface.png");
		DeviceTextureHandle texHandle2 = forwardRenderer.MutRHI()->TextureManager().CreateTexture2DFromFile(awesomeTexture);

		SamplerDescription samplerDesc;
		samplerDesc.m_anisotropy = SamplerDescription::AnisotropyLevel::Maximum;

		DeviceSamplerHandle sampler = forwardRenderer.MutRHI()->TextureManager().CreateSampler(samplerDesc);

		ResourceBindingList bindings{
										{0, BindingType::Sampler, ShaderStage::Fragment },
										{0, BindingType::TextureReadOnly, ShaderStage::Fragment },
										{1, BindingType::Sampler, ShaderStage::Fragment },
										{1, BindingType::TextureReadOnly, ShaderStage::Fragment } };
		passDesc.ResourceSetLayouts.AddResourceLayout(0, std::move(bindings));
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 0, texHandle);
		passDesc.ResourceBindings.EmplaceBinding<SamplerBinding>(0, 0, sampler);
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 1, texHandle2);
		passDesc.ResourceBindings.EmplaceBinding<SamplerBinding>(0, 1, sampler);

		Ref<MaterialResource> basicMat = rscSvc->EmplaceResource<MaterialResource>(HashString("BasicMaterial"), matDesc);

		auto* rdrSvc = EditEngine()->EditService<RenderService>();
		RenderScene& scene = rdrSvc->EmplaceScene(forwardRenderer);

		for (int i = 0; i < 10; i++)
		{
			m_cubes[i] = scene.AddObject(cubeMesh, basicMat);
		}

		m_svcTime = EditEngine()->EditService<TimeService>();

		m_scene = &scene;

		InputService* svcInput = EditEngine()->AddService<InputService>();
		auto* winSvc = EditEngine()->EditService<WindowService>();

		svcInput->AttachInputSource(*winSvc->MutWindow());

		Vec3 cameraPos{ 0, 1.f, 0.f };
		Vec3 cameraTarget{ Vec3::ZeroVector() };
		Vec3 up{ 0, 1, 0 };

		PerspectiveProjectionData perspective;
		perspective.FoVY = 45_degf;
		perspective.Near = 1.f;
		perspective.Far = 1000.f;
		svcInput->EmplaceController<FlyingCameraController>(scene, cameraPos, cameraTarget, up, perspective);

	}

	void CubeWorld::Update()
	{
		OpenGLApp3D::Update();

		float time = m_svcTime->GetSecondsSinceGameStart();

		static const Vec3 cubePositions[] =
		{
			Vec3(0.0f,  0.0f,  0.0f),
			Vec3(3.0f,  5.0f, -15.0f),
			Vec3(-3.f, -2.2f, -2.5f),
			Vec3(-3.8f,0.f, -12.3f),
			Vec3(2.4f, 0, -3.5f),
			Vec3(-1.7f,0.f, -7.5f),
			Vec3(1.3f, 0, -2.5f),
			Vec3(1.5f, 0, -2.5f),
			Vec3(1.5f, 0, -1.5f),
			Vec3(-1.3f,0.f, -1.5f)
		};

		for (int i = 0; i < 10; i++)
		{
			auto angle = Degs_f(time * 50);
			Mat4 cubeMatrix = Mat4::Translation(cubePositions[i]);// .Rotate(angle, 0.5f, 1.0f, 0.f);
			m_scene->UpdateObjectModel(m_cubes[i].GetID(), cubeMatrix);
		}
	}


	Renderer& CubeWorld::InitializeRenderer()
	{
		auto* rdrSvc = EditEngine()->EditService<RenderService>();

		Renderer& forwardRenderer = rdrSvc->EmplaceRenderer(rdrSvc->MutRHI());

		auto* winSvc = EditEngine()->EditService<WindowService>();
		forwardRenderer.AttachSurface(*winSvc->MutWindow());
		DeviceSwapchainHandle swapchain = forwardRenderer.MutRHI()->SwapchainManager().CreateSwapchain(forwardRenderer.MutSurface());

		forwardRenderer.EmplaceRenderPass<GeometryRenderPass>();

		forwardRenderer.EmplaceRenderPass<PresentRenderPass>(swapchain);

		return forwardRenderer;
	}
}
