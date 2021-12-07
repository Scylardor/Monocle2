#pragma once
#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{
	class RenderHardwareInterface;
	class IGraphicsSurface;

	struct Monocle_Graphics_API	DeviceSwapchainHandle : RenderableObjectHandle<ObjectPoolID>
	{
	private:
		static const ObjectPoolID ms_INVALID_ID = (ObjectPoolID) - 1;

	public:

		DeviceSwapchainHandle() = default;
		DeviceSwapchainHandle(ObjectPoolID handleID) :
			RenderableObjectHandle(handleID)
		{}

		static DeviceSwapchainHandle	Null() { return DeviceSwapchainHandle(); }

	};


	/*
	 * An abstract class for swap chain (mostly render targets of API-driven graphics surfaces) management.
	 */
	class ISwapchainManager
	{
	public:

		virtual ~ISwapchainManager() = default;

		virtual DeviceSwapchainHandle	CreateSwapchain(RenderHardwareInterface* RHI, IGraphicsSurface* boundSurface) = 0;

		virtual DeviceTextureHandle		GetMainSwapchainColorAttachment(uint32_t colorAttachmentIdx = 0) = 0;
		virtual DeviceTextureHandle		GetMainSwapchainDepthStencilAttachment() = 0;



		virtual	bool					Present(DeviceSwapchainHandle presentedSwapchain) = 0;
	};


}
