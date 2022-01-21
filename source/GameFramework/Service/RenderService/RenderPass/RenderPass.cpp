#include "RenderPass.h"


#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "Graphics/RHI/RenderHardwareInterface.h"
#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

namespace moe
{

	IRenderPass::IRenderPass(Renderer& owner, SurfaceDimensionsSync isResizingSynchronized) :
		m_renderer(&owner)
	{
 		if (isResizingSynchronized == SurfaceDimensionsSync::Yes)
			CreateSurfaceSynchronizedFramebuffer();
	}


	void IRenderPass::Destroy(RenderHardwareInterface& /*rhi*/)
	{
	}

	void IRenderPass::CreateFramebuffer(Renderer& renderer, std::pair<int, int> const& dimensions)
	{
		auto* RHI = renderer.MutRHI();
		auto& framebufferManager = RHI->FramebufferManager();
		m_framebuffer = framebufferManager.CreateFramebuffer(dimensions);

		ISwapchainManager& scManager = RHI->SwapchainManager();
		DeviceTextureHandle scColorAttach = scManager.GetMainSwapchainColorAttachment();
		DeviceTextureHandle scDepthAttach = scManager.GetMainSwapchainDepthStencilAttachment();

		framebufferManager.AddColorAttachment(m_framebuffer, scColorAttach);
		framebufferManager.SetDepthStencilAttachment(m_framebuffer, scDepthAttach);
	}


	void IRenderPass::CreateSurfaceSynchronizedFramebuffer()
	{
		auto* gfxSurface = m_renderer->MutSurface();
		MOE_DEBUG_ASSERT(gfxSurface != nullptr);
		auto dimensions = gfxSurface->GetDimensions();

		CreateFramebuffer(*m_renderer, dimensions);

		DeviceSwapchainHandle mainSwapchainHandle{ 0 };
		m_renderer->MutRHI()->SwapchainManager().RegisterSwapchainResizedCallback(
			mainSwapchainHandle,
			{ [this](int width, int height)
			{
				this->OnGraphicsSurfaceResized(width, height);
			} }
		);
	}


	void IRenderPass::OnGraphicsSurfaceResized(int newWidth, int newHeight)
	{
		auto* RHI = m_renderer->MutRHI();
		auto& framebufferManager = RHI->FramebufferManager();

		// dont destroy the attachments of the swap chain : the swap chain probably deleted them itself
		framebufferManager.DestroyFramebuffer(m_framebuffer, IFramebufferManager::DestroyAttachmentMode::KeepAttachments);

		m_framebuffer = framebufferManager.CreateFramebuffer({ newWidth, newHeight });

		ISwapchainManager& scManager = RHI->SwapchainManager();
		DeviceTextureHandle scColorAttach = scManager.GetMainSwapchainColorAttachment();
		DeviceTextureHandle scDepthAttach = scManager.GetMainSwapchainDepthStencilAttachment();

		framebufferManager.AddColorAttachment(m_framebuffer, scColorAttach);
		framebufferManager.SetDepthStencilAttachment(m_framebuffer, scDepthAttach);
	}
}
