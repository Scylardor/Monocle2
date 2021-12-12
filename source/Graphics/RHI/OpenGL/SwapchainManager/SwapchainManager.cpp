#include "SwapchainManager.h"


#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"
#include "Graphics/RHI/OpenGL/OGL4RHI.h"

namespace moe
{
	OpenGL4SwapchainManager::OpenGL4SwapchainManager(RenderHardwareInterface* RHI):
		m_RHI(dynamic_cast<OpenGL4RHI*>(RHI))
	{
	}

	DeviceSwapchainHandle OpenGL4SwapchainManager::CreateSwapchain(IGraphicsSurface* boundSurface)
	{
		if (m_RHI == nullptr)
		{
			MOE_FATAL(ChanGraphics, "OpenGL swapchain manager not used with OpenGL RHI : something is very wrong.");
			MOE_ASSERT(false);
			return DeviceSwapchainHandle::Null();
		}

		MOE_ASSERT(boundSurface != nullptr);
		if (boundSurface == nullptr)
		{
			MOE_ERROR(ChanGraphics, "OpenGL swapchain manager needs to have a valid graphics surface to create a swapchain.");
			return DeviceSwapchainHandle::Null();
		}

		auto dimensions = boundSurface->GetDimensions();

		OpenGL4FramebufferManager& fbManager = m_RHI->GLFramebufferManager();

		DeviceFramebufferHandle fbHandle = fbManager.CreateFramebuffer(dimensions);
		// TODO : Putting Sampled here as it is likely the swapchain color attachment be sampled to be rendered on a fullscreen quad for postprocess.
		// If we don't do it, materials trying to bind it won't work because the API is going to create a renderbuffer instead.
		// Double check if it is a good idea or not later.
		fbManager.CreateFramebufferColorAttachment(fbHandle, moe::TextureFormat::RGB8, TextureUsage((int)RenderTarget | (int)Sampled));
		fbManager.CreateDepthStencilAttachment(fbHandle);

		auto scID = m_swapchains.Emplace(boundSurface, fbHandle);
		auto swapChainHandle = DeviceSwapchainHandle(scID);

		boundSurface->OnSurfaceResizedEvent().AddDelegate({
			 [this, swapChainHandle](int width, int height)
			{
				this->OnSurfaceResized( swapChainHandle, width, height);
			}
		});

		return swapChainHandle;
	}


	DeviceFramebufferHandle OpenGL4SwapchainManager::GetMainSwapchainFramebufferHandle()
	{
		if (m_swapchains.Size() == 0)
			return DeviceFramebufferHandle::Null();

		return m_swapchains.Get(0).Framebuffer;
	}

	DeviceTextureHandle OpenGL4SwapchainManager::GetMainSwapchainColorAttachment(uint32_t colorAttachmentIdx)
	{
		if (m_swapchains.Size() == 0)
			return DeviceTextureHandle::Null();

		return m_RHI->GLFramebufferManager().GetFramebufferColorAttachment(m_swapchains.Get(0).Framebuffer, colorAttachmentIdx);
	}


	DeviceTextureHandle OpenGL4SwapchainManager::GetMainSwapchainDepthStencilAttachment()
	{
		if (m_swapchains.Size() == 0)
			return DeviceTextureHandle::Null();

		return m_RHI->GLFramebufferManager().GetFramebufferDepthStencilAttachment(m_swapchains.Get(0).Framebuffer);

	}


	EventDelegateID OpenGL4SwapchainManager::RegisterSwapchainResizedCallback(DeviceSwapchainHandle scHandle, SwapchainResizedEvent::DelegateType&& dlgt)
	{
		OpenGL4Swapchain& swapchain = m_swapchains.Mut(scHandle.Get());
		return swapchain.SwapchainResizedEvent.AddDelegate(std::move(dlgt));
	}


	bool OpenGL4SwapchainManager::Present(DeviceSwapchainHandle presentedSwapchain)
	{
		// OpenGL has no swapchain per se, and is 100% relying on the actual graphics surface to swap buffers.
		OpenGL4Swapchain& swapchain = m_swapchains.Mut(presentedSwapchain.Get());

		swapchain.Surface->SwapBuffers();

		return true;
	}


	void	OpenGL4SwapchainManager::OnSurfaceResized(DeviceSwapchainHandle swapchainHandle, int width, int height)
	{
		OpenGL4Swapchain& sc = m_swapchains.Mut(swapchainHandle.Get());
		OpenGL4FramebufferManager& fbManager = m_RHI->GLFramebufferManager();

		fbManager.ResizeFramebuffer(sc.Framebuffer, { width, height });

		glViewport(0, 0, width, height);

		sc.SwapchainResizedEvent.Broadcast(width, height);
	}
}
