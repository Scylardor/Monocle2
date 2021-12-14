#pragma once

#include "Core/Containers/ObjectPool/ObjectPool.h"
#include "Graphics/RHI/SwapchainManager/SwapchainManager.h"


namespace moe
{
	class OpenGL4RHI;
	class RenderHardwareInterface;
	class IGraphicsSurface;


	/* There's no such thing as a "swapchain" in OpenGL.
	 * So we have to emulate it, kind of...
	 * It will store everythin related to graphics surfaces render targets and their presentation.
	 */
	struct OpenGL4Swapchain
	{
		OpenGL4Swapchain() = default;
		OpenGL4Swapchain(IGraphicsSurface* surf, DeviceFramebufferHandle fbHandle) :
			Surface(surf), Framebuffer(fbHandle)
		{}

		IGraphicsSurface*		Surface = nullptr;
		DeviceFramebufferHandle	Framebuffer;
		SwapchainResizedEvent	SwapchainResizedEvent{};
	};


	/*
	 * An abstract class for swap chain (mostly render targets of API-driven graphics surfaces) management.
	 */
	class OpenGL4SwapchainManager : public ISwapchainManager
	{
	public:

		OpenGL4SwapchainManager(RenderHardwareInterface* RHI);

		DeviceSwapchainHandle	CreateSwapchain(IGraphicsSurface* boundSurface) override;

		DeviceFramebufferHandle	GetMainSwapchainFramebufferHandle() override;
		DeviceTextureHandle		GetMainSwapchainColorAttachment(uint32_t colorAttachmentIdx = 0) override;
		DeviceTextureHandle		GetMainSwapchainDepthStencilAttachment() override;

		EventDelegateID			RegisterSwapchainResizedCallback(DeviceSwapchainHandle scHandle, SwapchainResizedEvent::DelegateType&& dlgt) override;

		bool					Present(DeviceSwapchainHandle presentedSwapchain) override;

	private:

		void	OnSurfaceResized(DeviceSwapchainHandle swapchainHandle, int width, int height);


		OpenGL4RHI*							m_RHI;

		DynamicObjectPool<OpenGL4Swapchain>	m_swapchains{};

	};


}
