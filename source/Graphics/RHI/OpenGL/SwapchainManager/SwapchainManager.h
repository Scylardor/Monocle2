#pragma once

#include "Core/Containers/ObjectPool/ObjectPool.h"
#include "Graphics/RHI/OpenGL/FramebufferManager/FramebufferManager.h"
#include "Graphics/RHI/SwapchainManager/SwapchainManager.h"

namespace moe
{
	class RenderHardwareInterface;
	class IGraphicsSurface;


	/* There's no such thing as a "swapchain" in OpenGL.
	 * So we have to emulate it, kind of...
	 * It will store everythin related to graphics surfaces render targets and their presentation.
	 */
	struct OpenGLSwapchain
	{
		OpenGLSwapchain() = default;
		OpenGLSwapchain(IGraphicsSurface* surf, DeviceFramebufferHandle fbHandle) :
			Surface(surf), Framebuffer(fbHandle)
		{}

		IGraphicsSurface*		Surface = nullptr;
		DeviceFramebufferHandle	Framebuffer;
	};


	/*
	 * An abstract class for swap chain (mostly render targets of API-driven graphics surfaces) management.
	 */
	class OpenGL4SwapchainManager : public ISwapchainManager
	{
	public:

		OpenGL4SwapchainManager(OpenGL4FramebufferManager& fbManager)  :
			m_fbManager(fbManager)
		{}

		DeviceSwapchainHandle	CreateSwapchain(RenderHardwareInterface* RHI, IGraphicsSurface* boundSurface) override;

		bool					Present(DeviceSwapchainHandle presentedSwapchain) override;

	private:

		void	OnSurfaceResized(DeviceSwapchainHandle swapchainHandle, int width, int height);


		OpenGL4FramebufferManager&			m_fbManager;

		DynamicObjectPool<OpenGLSwapchain>	m_swapchains{};

	};


}
