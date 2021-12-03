#include "FramebufferManager.h"

namespace moe
{
	DeviceFramebufferHandle OpenGL4FramebufferManager::CreateFramebuffer()
	{
		return DeviceFramebufferHandle();
	}
	DeviceFramebufferHandle OpenGL4FramebufferManager::CreateFramebufferColorAttachment(uint32_t /*width*/, uint32_t /*height*/, TextureFormat /*format*/)
	{
		return DeviceFramebufferHandle();
	}

	DeviceFramebufferHandle OpenGL4FramebufferManager::CreateDepthStencilAttachment(uint32_t /*width*/, uint32_t /*height*/, TextureFormat /*format*/)
	{
		return DeviceFramebufferHandle::Null();
	}
}
