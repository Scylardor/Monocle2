#include "PresentPass.h"

#include "../Renderer/Renderer.h"
#include "Core/HashString/HashString.h"
#include "Graphics/RHI/RenderHardwareInterface.h"
#include "Graphics/Vertex/VertexFormats.h"

#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"
#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/ResourceService/ResourceService.h"

#include "Graphics/RenderQueue/RenderQueue.h"

namespace moe
{
	PresentRenderPass::PresentRenderPass(Renderer& owner, DeviceSwapchainHandle attachedSwapchain) :
		m_ownerRenderer(&owner),
		m_swapchain(attachedSwapchain)
	{
		auto* RHI = owner.MutRHI();

		m_framebuffer = RHI->SwapchainManager().GetMainSwapchainFramebufferHandle();

		DeviceTextureHandle swapchainColorTex = RHI->SwapchainManager().GetMainSwapchainColorAttachment();

		const Array<Vertex2D_PosUV, 6> fullscreenQuad
		{
			// positions   // texCoords
			{{-1.0f,  1.0f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f},  {0.0f, 0.0f}},
			{{ 1.0f, -1.0f},  {1.0f, 0.0f}},
			{{-1.0f,  1.0f},  {0.0f, 1.0f}},
			{{ 1.0f, -1.0f},  {1.0f, 0.0f}},
			{{ 1.0f,  1.0f},  {1.0f, 1.0f}}
		};

		MeshData quadData = MeshData::Build(fullscreenQuad);
		m_fullscreenQuadMesh = RHI->BufferManager().FindOrCreateMeshBuffer(quadData);

		auto* engine = m_ownerRenderer->MutRenderService()->EditEngine();
		auto* rscSvc = engine->EditService<ResourceService>();

		auto basicVertShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("FullscreenQuad.vert"), "source/Graphics/Resources/shaders/OpenGL/fullscreen_quad2.vert", FileMode::Text);
		auto basicFragShaderFile = rscSvc->EmplaceResource<FileResource>(HashString("FullscreenQuad.frag"), "source/Graphics/Resources/shaders/OpenGL/fullscreen_quad2.frag", FileMode::Text);

		MaterialDescription matDesc;
		MaterialPassDescription& passDesc = matDesc.NewPassDescription();
		passDesc.AssignPipelineVertexLayout({ {
			{"v_Pos", VertexBindingFormat::Float2 },
			{"v_UV0", VertexBindingFormat::Float2 }}
		}).
		AssignShaderProgramDescription({
			{	ShaderStage::Vertex, basicVertShaderFile},
			{	ShaderStage::Fragment, basicFragShaderFile}
		}).
		Pipeline.DepthStencilStateDesc.SetDepthTestEnabled(DepthTest::Disabled, DepthWriting::Disabled);

		ResourceBindingList bindings;
		bindings.EmplaceBack(0, BindingType::TextureReadOnly, ShaderStage::Fragment);
		passDesc.ResourceSetLayouts.AddResourceLayout(0, std::move(bindings));
		passDesc.ResourceBindings.EmplaceBinding<TextureBinding>(0, 0, swapchainColorTex);

		m_fullscreenQuadMaterial = RHI->MaterialManager().CreateMaterial(matDesc);
	}


	void PresentRenderPass::Update(RenderQueue& drawQueue, uint8_t passIndex)
	{
		RenderQueueKey key = RenderQueue::ComputeRenderQueueKey(passIndex);
		key = drawQueue.EmplaceCommand<CmdBeginRenderPass>(key, m_framebuffer, ColorRGBAf::Red());

		auto [matIdx, programIdx] = m_fullscreenQuadMaterial.DecomposeMaterialAndShaderIndices();

		key.Material = matIdx;
		key.Program = programIdx;

		// Make sure to reset the viewport to draw on the whole surface
		// Otherwise we could be still using a viewport from a previous pass!
		auto [surfaceWidth, surfaceHeight] = m_ownerRenderer->GetSurface()->GetDimensions();
		Rect2Du fullscreenViewport{ 0, 0, (uint32_t) surfaceWidth, (uint32_t) surfaceHeight };

		drawQueue.EmplaceDrawCall<CmdSetViewportScissor>(key, fullscreenViewport, fullscreenViewport);

		drawQueue.EmplaceDrawCall<CmdBindMaterial>(key, m_fullscreenQuadMaterial);

		drawQueue.EmplaceDrawCall<CmdDrawMesh>(key, m_fullscreenQuadMesh);

		key = drawQueue.EmplaceCommand<CmdEndRenderPass>(key);

		key = drawQueue.EmplaceCommand<CmdPresentSwapchain>(key, m_swapchain);


	}
}
