// Monocle Game Engine source files - Alexandre Baron

#include "BasicQuad.h"


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

namespace moe
{

	void BasicQuad::Start()
	{
		OpenGLApp3D::Start();

		auto* rscSvc = EditEngine()->AddService<ResourceService>();

		Array<Vertex_PosColorUV, 4> helloQuad{
		{{0.5f, 0.5f, 0.0f } , {1.0f, 0.0f, 0.0f }, {1.0f, 1.0f} },   // top right
		{{0.5f, -0.5f, 0.0f } , {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // bottom right
		{{-0.5f, -0.5f, 0.0f} , {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},   // bottom left
		{{-0.5f, 0.5f, 0.0f } , {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}    // top left
		};

		Array<uint32_t, 6> helloIndices = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};

		auto meshRsc = rscSvc->EmplaceResource<MeshResource>(
			HashString("QuadMesh"),
			MeshData{ helloQuad.Data(), sizeof(helloQuad[0]), helloQuad.Size(),
			helloIndices.Data(), sizeof(helloIndices[0]), helloIndices.Size() });

		auto basicVertShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BasicShader.vert"), "source/Graphics/Resources/shaders/OpenGL/basic_textured.vert", FileMode::Text);
		auto basicFragShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BasicShader.frag"), "source/Graphics/Resources/shaders/OpenGL/basic_textured.frag", FileMode::Text);

		MaterialDescription matDesc;
		auto& passDesc = matDesc.NewPassDescription();
		passDesc.
			AssignPipelineVertexLayout({ {
				{"position", VertexBindingFormat::Float3 },
				{"color", VertexBindingFormat::Float3 },
				{"uv0", VertexBindingFormat::Float2 } }
			}).AssignShaderProgramDescription({
			{	ShaderStage::Vertex, basicVertShaderFile},
			{	ShaderStage::Fragment, basicFragShaderFile}
			})
		.Pipeline.RasterizerStateDesc.m_cullMode = CullFace::None;
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

		m_quad = scene.AddObject(meshRsc, basicMat);

		Mat4 quadTrans = Mat4::Rotation(90_degf, Vec3(0, 0, 1));
		quadTrans.Scale(Vec3(0.5f));

		scene.SetObjectTransform(m_quad.GetID(), quadTrans);

		m_svcTime = EditEngine()->EditService<TimeService>();

		m_scene = &scene;


		InputService* svcInput = EditEngine()->AddService<InputService>();
		auto* winSvc = EditEngine()->EditService<WindowService>();

		svcInput->AttachInputSource(*winSvc->MutWindow());

		Vec3 cameraPos{ 0, 0, 3 };
		Vec3 cameraTarget{ Vec3::ZeroVector() };
		Vec3 up{ 0, 1, 0 };

		PerspectiveProjectionData perspective;
		perspective.FoVY = 45_degf;
		perspective.Near = 1.f;
		perspective.Far = 1000.f;
		svcInput->EmplaceController<FlyingCameraController>(scene, cameraPos, cameraTarget, up, perspective);
	}


	void BasicQuad::Update()
	{
		OpenGLApp3D::Update();

		float time = m_svcTime->GetSecondsSinceGameStart();
		Mat4 modelMatrix = Mat4::Rotation(Degs_f(time * 10), Vec3(0, 0, 1));

		m_quad.MutObject()->SetModelMatrix(modelMatrix);
	}


	Renderer& BasicQuad::InitializeRenderer()
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
