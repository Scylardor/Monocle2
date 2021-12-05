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
