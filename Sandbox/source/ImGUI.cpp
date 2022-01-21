// Monocle Game Engine source files - Alexandre Baron

#include "ImGUI.h"


#include "Core/Containers/Array/Array.h"
#include "Core/Resource/FileResource.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Core/Resource/MeshResource.h"
#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/InputService/InputService.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/RenderService/RenderPass/GeometryRenderPass.h"
#include "GameFramework/Service/RenderService/RenderPass/ImGUIPass.h"
#include "GameFramework/Service/RenderService/RenderPass/PresentPass.h"
#include "GameFramework/Service/ResourceService/ResourceService.h"
#include "GameFramework/Service/WindowService/WindowService.h"
#include "Graphics/Sampler/SamplerDescriptor.h"
#include "Graphics/ShaderCapabilities/ShaderCapabilities.h"
#include "Graphics/Vertex/VertexFormats.h"
#include "Graphics/Geometry/Cube.h"
#include "GameFramework/Service/TimeService/TimeService.h"

namespace moe
{

	void ImGUI::Start()
	{
		OpenGLApp3D::Start();

		auto* rscSvc = EditEngine()->AddService<ResourceService>();

		auto ImGUIVertShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BlinnPhong.vert"), "source/Graphics/Resources/shaders/OpenGL/blinn_phong_v2.vert", FileMode::Text);
		auto ImGUIFragShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BlinnPhong.frag"), "source/Graphics/Resources/shaders/OpenGL/blinn_phong_v2.frag", FileMode::Text);

		VertexLayoutDescription vertexLayout = { {
			{ "position", VertexBindingFormat::Float3 },
			{ "normal", VertexBindingFormat::Float3},
			{ "uv0", VertexBindingFormat::Float2 } } };

		ShaderProgramDescription shaderDesc{
			{
				{	ShaderStage::Vertex, ImGUIVertShaderFile},
				{	ShaderStage::Fragment, ImGUIFragShaderFile}
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

		Ref<TextureResource> whiteTex = rscSvc->EmplaceResource<TextureResource>(HashString("Tex_White"), "Sandbox/assets/textures/white.png");
		DeviceTextureHandle whiteTexHandle = forwardRenderer.MutRHI()->TextureManager().CreateTexture2DFromFile(whiteTex);

		SamplerDescription samplerDesc;
		samplerDesc.m_anisotropy = SamplerDescription::AnisotropyLevel::Maximum;

		DeviceSamplerHandle sampler = forwardRenderer.MutRHI()->TextureManager().CreateSampler(samplerDesc);

		ResourceBindingList bindings{
										{0, BindingType::Sampler, ShaderStage::Fragment },
										{0, BindingType::TextureReadOnly, ShaderStage::Fragment } };
		passDesc.ResourceSetLayouts.AddResourceLayout(0, std::move(bindings));
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 0, whiteTexHandle);
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 1, texHandle);
		passDesc.ResourceBindings.EmplaceBinding<SamplerBinding>(0, 1, sampler);
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 2, whiteTexHandle);
		passDesc.AddShaderCapability<SC_PhongLighting>();

		Ref<MaterialResource> woodMat = rscSvc->EmplaceResource<MaterialResource>(HashString("Mat_Wood"), matDesc);

		Ref<TextureResource> containerTex = rscSvc->EmplaceResource<TextureResource>(HashString("Tex_Container"), "Sandbox/assets/textures/container2.png");
		texHandle = forwardRenderer.MutRHI()->TextureManager().CreateTexture2DFromFile(containerTex);

		passDesc.ResourceBindings.Clear();
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 0, whiteTexHandle);
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 1, texHandle);
		passDesc.ResourceBindings.EmplaceBinding<SamplerBinding>(0, 1, sampler);
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 2, whiteTexHandle);
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


		Mat4 modelMatrices[NUM_CUBES]{
			Mat4::Translation(0, 1.5f, 3) * Mat4::Scaling(Vec3(0.5f)),
			Mat4::Translation(5, 3, 1) * Mat4::Scaling(Vec3(3.f)),
			Mat4::Translation(-1, 5, 2) * Mat4::Rotation(60_degf, Vec3(1.f, 0.f, 1.f).GetNormalized()) * Mat4::Scaling(Vec3(0.25f)),
			Mat4::Identity(),
			Mat4::Translation(10, 3, 1) * Mat4::Scaling(Vec3(0.5f))
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

		Vec3 cameraPos{ 0, 1, 3 };
		Vec3 cameraTarget{ Vec3::ZeroVector() };
		Vec3 up{ 0, 1, 0 };

		PerspectiveProjectionData perspective;
		perspective.FoVY = 45_degf;
		perspective.Near = 1.f;
		perspective.Far = 1000.f;
		m_camCtrl = svcInput->EmplaceController<FlyingCameraController>(scene, cameraPos, cameraTarget, up, perspective);

		PointLight pl;
		pl.Position = Vec3(0, 5, 0);
		pl.ConstantAttenuation = 1;
		pl.LinearAttenuation = 0.05f;
		pl.QuadraticAttenuation = 0.015f;
		pl.Ambient = Color3f(0.05f, 0.05f, 0.05f);
		m_pointLightID = scene.AddLight(pl);

		DirectionalLight dl;
		dl.Direction = Vec3(0, 0, 1);
		dl.Diffuse = Color3f::White();
		dl.Specular = Color3f::Black();
		m_DirLightID = scene.AddLight(dl);

		SpotLight sl;
		sl.Position = Vec3(10, 3, 1);
		sl.Direction = Vec3(1, 0, 0);
		sl.Diffuse = Color3f::Yellow();
		sl.Specular = Color3f(0.5f, 0.5f, 0);
		sl.InnerCutoff = Degs_f{ 12.5 };
		sl.OuterCutoff = Degs_f{ 15.F };
		sl.ConstantAttenuation = 1;
		sl.LinearAttenuation = 0.09f;
		sl.QuadraticAttenuation = 0.032f;
		m_SpotLightID = scene.AddLight(sl);

		m_svcTime = EditEngine()->EditService<TimeService>();

		svcInput->MutKeyActionEvent(MOE_KEY(K), ButtonPressedState::Released).Add<ImGUI, &ImGUI::ToggleCursor>(this);
	}

	void ImGUI::Update()
	{
		OpenGLApp3D::Update();

		float time = m_svcTime->GetSecondsSinceGameStart();

		float sin = std::sinf(time);
		Vec3 lightPos = Vec3(0, 5 + sin * 4, 0);

		m_scene->UpdateObjectModel(m_cubes[NUM_CUBES - 1].GetID(), Mat4::Translation(lightPos));
		m_scene->UpdateLightPosition(m_pointLightID, lightPos);
		MOE_LOG("sine: %f", sin);

		lightPos = m_camCtrl->GetCameraPosition();
		Vec3 lightDir = m_camCtrl->GetCameraFront();

		m_scene->UpdateLightTransform(m_SpotLightID, lightPos, lightDir);
	}


	Renderer& ImGUI::InitializeRenderer()
	{
		auto* rdrSvc = EditEngine()->EditService<RenderService>();

		Renderer& forwardRenderer = rdrSvc->EmplaceRenderer(rdrSvc->MutRHI());

		auto* winSvc = EditEngine()->EditService<WindowService>();
		forwardRenderer.AttachSurface(*winSvc->MutWindow());
		DeviceSwapchainHandle swapchain = forwardRenderer.MutRHI()->SwapchainManager().CreateSwapchain(forwardRenderer.MutSurface());

		IRenderPass& grp = forwardRenderer.EmplaceRenderPass<IRenderPass>();
		grp.EmplaceSubpass<GeometryRenderPass>(forwardRenderer);
		grp.EmplaceSubpass<ImGUIPass>(forwardRenderer);

		forwardRenderer.EmplaceRenderPass<PresentRenderPass>(swapchain);

		return forwardRenderer;
	}


	void ImGUI::ToggleCursor()
	{
		auto* winSvc = EditEngine()->EditService<WindowService>();
		winSvc->MutWindow()->SetCursorMode(CursorMode::Normal);
	}
}
