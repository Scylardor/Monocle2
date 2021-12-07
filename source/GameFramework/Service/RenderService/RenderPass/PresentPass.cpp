#include "PresentPass.h"

#include "../Renderer/Renderer.h"
#include "Core/HashString/HashString.h"
#include "Graphics/RHI/RenderHardwareInterface.h"
#include "Graphics/Vertex/VertexFormats.h"

#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/ResourceService/ResourceService.h"

namespace moe
{
	PresentRenderPass::PresentRenderPass(Renderer& owner) :
		m_ownerRenderer(&owner)
	{
		auto* RHI = owner.MutRHI();
		m_swapchain = RHI->SwapchainManager().CreateSwapchain(RHI, owner.MutSurface());
		DeviceTextureHandle swapchainColorTex = RHI->SwapchainManager().GetMainSwapchainColorAttachment();

		Array<Vertex2D_PosUV, 6> fullscreenQuad
		{
			// positions   // texCoords
			{{-1.0f,  1.0f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f},  {0.0f, 0.0f}},
			{{ 1.0f, -1.0f},  {1.0f, 0.0f}},
			{{-1.0f,  1.0f},  {0.0f, 1.0f}},
			{{ 1.0f, -1.0f},  {1.0f, 0.0f}},
			{{ 1.0f,  1.0f},  {1.0f, 1.0f}}
		};

		auto* engine = m_ownerRenderer->MutRenderService()->EditEngine();
		auto* rscSvc = engine->EditService<ResourceService>();

		auto basicVertShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("FullscreenQuad.vert"), "source/Graphics/Resources/shaders/OpenGL/fullscreen_quad2.vert", FileMode::Text);
		auto basicFragShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("FullscreenQuad.frag"), "source/Graphics/Resources/shaders/OpenGL/fullscreen_quad2.frag", FileMode::Text);

		MaterialDescription matDesc;
		MaterialPassDescription& passDesc = matDesc.NewPassDescription();
		passDesc.AssignShaderProgramDescription({
				{	ShaderStage::Vertex, basicVertShaderFile},
				{	ShaderStage::Fragment, basicFragShaderFile}
			})
			.Pipeline.DepthStencilStateDesc.SetDepthTestEnabled(DepthTest::Disabled, DepthWriting::Disabled);

		ResourceBindingList bindings;
		bindings.EmplaceBack(0, BindingType::TextureReadOnly, ShaderStage::Fragment);
		passDesc.ResourceSetLayouts.AddResourceLayout(0, std::move(bindings));
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(swapchainColorTex, 0, 0);

		RHI->MaterialManager().CreateMaterial(matDesc);

	}


	void PresentRenderPass::Update()
	{
		m_ownerRenderer->MutRHI()->SwapchainManager().Present(m_swapchain);
	}
}
