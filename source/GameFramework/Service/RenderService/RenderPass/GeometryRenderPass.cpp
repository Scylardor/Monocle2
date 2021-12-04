// Monocle source files - Alexandre Baron

#include "GeometryRenderPass.h"

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

#include "Graphics/RHI/RenderHardwareInterface.h"

namespace moe
{
	GeometryRenderPass::GeometryRenderPass(Renderer& owner) :
		IRenderPass(*owner.MutRHI()),
		m_ownerRenderer(&owner)
	{
		// We wanna resize our framebuffer everytime the graphics surface resizes itself.
		auto * gfxSurface = m_ownerRenderer->MutSurface();
		MOE_DEBUG_ASSERT(gfxSurface != nullptr);
		m_surfaceResizeDelID = gfxSurface->OnSurfaceResizedEvent().Add<GeometryRenderPass, &GeometryRenderPass::OnGraphicsSurfaceResized>(this);

		auto* RHI = m_ownerRenderer->MutRHI();
		auto& framebufferManager = RHI->FramebufferManager();

		auto dimensions = gfxSurface->GetDimensions();
		DeviceFramebufferHandle fbHandle = framebufferManager.CreateFramebuffer(dimensions);

		framebufferManager.CreateFramebufferColorAttachment(fbHandle);
		framebufferManager.CreateDepthStencilAttachment(fbHandle);

		m_swapchain = RHI->SwapchainManager().CreateSwapchain(RHI, m_ownerRenderer->MutSurface());
	}


	GeometryRenderPass::~GeometryRenderPass()
	{
		auto* gfxSurface = m_ownerRenderer->MutSurface();
		gfxSurface->OnSurfaceResizedEvent().Remove(m_surfaceResizeDelID);
	}


	void GeometryRenderPass::Update()
	{
		m_ownerRenderer->MutRHI()->SwapchainManager().Present(m_swapchain);
	}


	void GeometryRenderPass::OnGraphicsSurfaceResized(int /*newWidth*/, int /*newHeight*/)
	{

	}
}
