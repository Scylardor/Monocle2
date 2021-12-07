#include "SwapchainManager.h"

#include "Graphics/RHI/OpenGL/OGL4RHI.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

namespace moe
{

	DeviceSwapchainHandle OpenGL4SwapchainManager::CreateSwapchain(RenderHardwareInterface* RHI, IGraphicsSurface* boundSurface)
	{
		auto* rhi = dynamic_cast<OpenGL4RHI*>(RHI);
		if (rhi == nullptr)
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

		DeviceFramebufferHandle fbHandle = m_fbManager.CreateFramebuffer(dimensions);
		m_fbManager.CreateFramebufferColorAttachment(fbHandle, moe::TextureFormat::RGB32F, TextureUsage::RenderTarget);
		m_fbManager.CreateDepthStencilAttachment(fbHandle);

		auto scID = m_swapchains.Emplace(boundSurface, fbHandle);
		auto swapChainHandle = DeviceSwapchainHandle(scID);

		using namespace std::placeholders;
		boundSurface->OnSurfaceResizedEvent().AddDelegate(
			{ std::bind(&OpenGL4SwapchainManager::OnSurfaceResized, this, swapChainHandle, _1, _2) }
		);

		return swapChainHandle;
	}

	DeviceTextureHandle OpenGL4SwapchainManager::GetMainSwapchainColorAttachment(uint32_t colorAttachmentIdx)
	{
		if (m_swapchains.Size() == 0)
			return DeviceTextureHandle::Null();

		return m_fbManager.GetFramebufferColorAttachment(m_swapchains.Get(0).Framebuffer, colorAttachmentIdx);
	}


	DeviceTextureHandle OpenGL4SwapchainManager::GetMainSwapchainDepthStencilAttachment()
	{
		if (m_swapchains.Size() == 0)
			return DeviceTextureHandle::Null();

		return m_fbManager.GetFramebufferDepthStencilAttachment(m_swapchains.Get(0).Framebuffer);

	}


	bool OpenGL4SwapchainManager::Present(DeviceSwapchainHandle presentedSwapchain)
	{
		// OpenGL has no swapchain per se, and is 100% relying on the actual graphics surface to swap buffers.
		OpenGLSwapchain& swapchain = m_swapchains.Mut(presentedSwapchain.Get());

		swapchain.Surface->SwapBuffers();

		return true;
	}

	void	OpenGL4SwapchainManager::OnSurfaceResized(DeviceSwapchainHandle swapchainHandle, int width, int height)
	{
		swapchainHandle;
		width;
		height;
	}
}
