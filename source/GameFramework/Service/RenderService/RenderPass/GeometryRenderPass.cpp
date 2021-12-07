// Monocle source files - Alexandre Baron

#include "GeometryRenderPass.h"

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

#include "Graphics/RHI/RenderHardwareInterface.h"

namespace moe
{
	GeometryRenderPass::GeometryRenderPass(Renderer& owner) :
		m_ownerRenderer(&owner)
	{
		auto* RHI = m_ownerRenderer->MutRHI();

		DeviceTextureHandle scColorAttach = RHI->SwapchainManager().GetMainSwapchainColorAttachment();
		DeviceTextureHandle scDepthAttach = RHI->SwapchainManager().GetMainSwapchainDepthStencilAttachment();

		auto* gfxSurface = m_ownerRenderer->MutSurface();
		MOE_DEBUG_ASSERT(gfxSurface != nullptr);
		auto dimensions = gfxSurface->GetDimensions();
		auto& framebufferManager = RHI->FramebufferManager();
		DeviceFramebufferHandle fbHandle = framebufferManager.CreateFramebuffer(dimensions);

		framebufferManager.AddColorAttachment(fbHandle, scColorAttach);
		framebufferManager.SetDepthStencilAttachment(fbHandle, scDepthAttach);
	}


	GeometryRenderPass::~GeometryRenderPass()
	{
		auto* gfxSurface = m_ownerRenderer->MutSurface();
		gfxSurface->OnSurfaceResizedEvent().Remove(m_surfaceResizeDelID);
	}


	void GeometryRenderPass::Update()
	{
	}


	void GeometryRenderPass::OnGraphicsSurfaceResized(int /*newWidth*/, int /*newHeight*/)
	{

	}
}
