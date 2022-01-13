// Monocle Game Engine source files - Alexandre Baron

#include "BlinnPhong.h"


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

	void BlinnPhong::Start()
	{
		OpenGLApp3D::Start();

		auto* rscSvc = EditEngine()->AddService<ResourceService>();

		auto blinnPhongVertShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BlinnPhong.vert"), "source/Graphics/Resources/shaders/OpenGL/blinn_phong_v2.vert", FileMode::Text);
		auto blinnPhongFragShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BlinnPhong.frag"), "source/Graphics/Resources/shaders/OpenGL/blinn_phong_v2.frag", FileMode::Text);

		VertexLayoutDescription vertexLayout = { {
			{ "position", VertexBindingFormat::Float3 },
			{ "normal", VertexBindingFormat::Float3},
			{ "uv0", VertexBindingFormat::Float2 } } };

		ShaderProgramDescription shaderDesc{
			{
				{	ShaderStage::Vertex, blinnPhongVertShaderFile},
				{	ShaderStage::Fragment, blinnPhongFragShaderFile}
			} };

		MaterialDescription matDesc;
		auto& passDesc = matDesc.NewPassDescription();
		passDesc
		.AssignPipelineVertexLayout(vertexLayout)
		.AssignShaderProgramDescription(shaderDesc)
		.Pipeline.RasterizerStateDesc.m_cullMode = CullFace::Back;

		passDesc.AddShaderCapability<SC_ObjectTransform>();

		Renderer& forwardRenderer = InitializeRenderer();

		Ref<TextureResource> woodTex = rscSvc->EmplaceResource<TextureResource>(HashString("Tex_Wood"), "Sandbox/assets/textures/wood.png");
		DeviceTextureHandle texHandle = forwardRenderer.MutRHI()->TextureManager().CreateTexture2DFromFile(woodTex);

		SamplerDescription samplerDesc;
		samplerDesc.m_anisotropy = SamplerDescription::AnisotropyLevel::Maximum;

		DeviceSamplerHandle sampler = forwardRenderer.MutRHI()->TextureManager().CreateSampler(samplerDesc);

		ResourceBindingList bindings{
										{0, BindingType::Sampler, ShaderStage::Fragment },
										{0, BindingType::TextureReadOnly, ShaderStage::Fragment } };
		passDesc.ResourceSetLayouts.AddResourceLayout(0, std::move(bindings));
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 0, texHandle);
		passDesc.ResourceBindings.EmplaceBinding<SamplerBinding>(0, 0, sampler);
		passDesc.AddShaderCapability<SC_PhongLighting>();

		Ref<MaterialResource> woodMat = rscSvc->EmplaceResource<MaterialResource>(HashString("Mat_Wood"), matDesc);

		Ref<TextureResource> containerTex = rscSvc->EmplaceResource<TextureResource>(HashString("Tex_Container"), "Sandbox/assets/textures/container2.png");
		texHandle = forwardRenderer.MutRHI()->TextureManager().CreateTexture2DFromFile(containerTex);

		passDesc.ResourceBindings.Clear();
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 0, texHandle);
		passDesc.ResourceBindings.EmplaceBinding<SamplerBinding>(0, 0, sampler);
		Ref<MaterialResource> containerMat = rscSvc->EmplaceResource<MaterialResource>(HashString("Mat_Container"), matDesc);

		auto* rdrSvc = EditEngine()->EditService<RenderService>();
		RenderScene& scene = rdrSvc->EmplaceScene(forwardRenderer);

		auto cubeGeometry = CreateCubePositionNormalTexture(0.5f, false);

		auto cubeMesh = rscSvc->EmplaceResource<MeshResource>(
			HashString("CubeMesh"),
			MeshData{ cubeGeometry.Data(), sizeof(cubeGeometry[0]), cubeGeometry.Size() });

		// vertex attributes for a unit plane
		Array<Vertex_PosNormalUV, 6> plane = {
			// positions   // normals // texCoords
			{{-0.5f, 0,  0.5f}, {0, 1, 0},  {0.0f, 1.0f}},
			{{ 0.5f, 0, -0.5f}, {0, 1, 0},  {1.0f, 0.0f}},
			{{-0.5f, 0, -0.5f}, {0, 1, 0},  {0.0f, 0.0f}},
			{{-0.5f, 0,  0.5f}, {0, 1, 0},  {0.0f, 1.0f}},
			{{ 0.5f, 0,  0.5f}, {0, 1, 0},  {1.0f, 1.0f}},
			{{ 0.5f, 0, -0.5f}, {0, 1, 0},  {1.0f, 0.0f}}
		};

		auto planeMesh = rscSvc->EmplaceResource<MeshResource>(
			HashString("UnitQuad"),
			MeshData{ plane.Data(), sizeof(plane[0]), plane.Size() });


		Mat4 modelMatrices[] = {
			Mat4::Translation(0, 1.5f, 0) * Mat4::Scaling(Vec3(0.5f)),
			Mat4::Translation(2, 3, 1) * Mat4::Scaling(Vec3(0.5f)),
			Mat4::Translation(-1, 5, 2) * Mat4::Rotation(60_degf, Vec3(1.f, 0.f, 1.f).GetNormalized()) * Mat4::Scaling(Vec3(0.25f))
		};

		for (int i = 0; i < NUM_CUBES; i++)
		{
			m_cubes[i] = scene.AddObject(cubeMesh, containerMat);
			scene.UpdateObjectModel(m_cubes[i].GetID(), modelMatrices[i]);
		}

		m_floor = scene.AddObject(planeMesh, woodMat);
		scene.UpdateObjectModel(m_floor.GetID(), Mat4::Scaling(Vec3{ 50 }));

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

	void BlinnPhong::Update()
	{
		OpenGLApp3D::Update();

	}


	Renderer& BlinnPhong::InitializeRenderer()
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
