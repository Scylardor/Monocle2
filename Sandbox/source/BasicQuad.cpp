// Monocle Game Engine source files - Alexandre Baron

#include "BasicQuad.h"


#include "Core/Containers/Array/Array.h"
#include "Core/Resource/FileResource.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Core/Resource/MeshResource.h"
#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/RenderService/RenderPass/GeometryRenderPass.h"
#include "GameFramework/Service/RenderService/RenderPass/PresentPass.h"
#include "GameFramework/Service/ResourceService/ResourceService.h"
#include "GameFramework/Service/WindowService/WindowService.h"
#include "Graphics/Vertex/VertexFormats.h"


namespace moe
{

	void BasicQuad::Start()
	{
		OpenGLApp3D::Start();

		auto* rscSvc = EditEngine()->AddService<ResourceService>();
		//auto* render = EditEngine()->EditService<RenderService>();

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

		auto basicVertShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BasicShader.vert"), "source/Graphics/Resources/shaders/OpenGL/basic.vert", FileMode::Text);
		auto basicFragShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("BasicShader.frag"), "source/Graphics/Resources/shaders/OpenGL/basic.frag", FileMode::Text);

		MaterialDescription matDesc;
		matDesc.NewPassDescription().
			AssignShaderProgramDescription({
			{	ShaderStage::Vertex, basicVertShaderFile},
			{	ShaderStage::Fragment, basicFragShaderFile}
		});

		rscSvc->EmplaceResource<MaterialResource>(HashString("BasicMaterial"), matDesc);

		Renderer& forwardRenderer = InitializeRenderer();

		auto* rdrSvc = EditEngine()->EditService<RenderService>();

		RenderScene& mainScene = rdrSvc->EmplaceScene(forwardRenderer);
		mainScene.AddObject(meshRsc, {});

		// rdrSvc->MutRHI()->BufferManager().FindOrCreateMeshBuffer(meshRsc);

		//Renderer& forwardRenderer = rdrSvc->EmplaceRenderer();




		//MaterialData myDefaultMaterial;


		//render->
	}


	Renderer& BasicQuad::InitializeRenderer()
	{
		auto* rdrSvc = EditEngine()->EditService<RenderService>();

		Renderer& forwardRenderer = rdrSvc->EmplaceRenderer(rdrSvc->MutRHI());

		auto* winSvc = EditEngine()->EditService<WindowService>();
		forwardRenderer.AttachSurface(*winSvc->MutWindow());

		forwardRenderer.EmplaceRenderPass<PresentRenderPass>();
		forwardRenderer.EmplaceRenderPass<GeometryRenderPass>();

		return forwardRenderer;
	}
}
