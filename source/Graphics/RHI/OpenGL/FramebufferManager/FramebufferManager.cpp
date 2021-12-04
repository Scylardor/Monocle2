#include "FramebufferManager.h"

#include "../TextureManager/TextureManager.h"

namespace moe
{
	DeviceFramebufferHandle OpenGL4FramebufferManager::CreateFramebuffer(std::pair<int, int> const& dimensions, uint32_t numSamples)
	{
		GLuint fbID;
		glCreateFramebuffers(1, &fbID);
		MOE_ASSERT(fbID < moe::MaxValue<DeviceFramebufferHandle::Underlying>());
		auto fbHandle = m_frameBuffers.Emplace(fbID, dimensions, numSamples);

		return DeviceFramebufferHandle(fbHandle);
	}


	DeviceTextureHandle OpenGL4FramebufferManager::CreateFramebufferColorAttachment(DeviceFramebufferHandle fbHandle, TextureFormat format, TextureUsage usage)
	{
		auto& fb = MutFramebuffer(fbHandle);

		TextureData colorAttachmentData;
		colorAttachmentData.Width = fb.Width;
		colorAttachmentData.Height = fb.Height;
		colorAttachmentData.Format = format;
		colorAttachmentData.Usage = usage;
		colorAttachmentData.Samples = fb.Samples;

		DeviceTextureHandle colorAttachment = m_texManager.CreateTexture2D(colorAttachmentData);
		fb.ColorAttachments.EmplaceBack(colorAttachment);
		return colorAttachment;
	}


	DeviceTextureHandle OpenGL4FramebufferManager::CreateDepthStencilAttachment(DeviceFramebufferHandle fbHandle, TextureFormat format, TextureUsage usage)
	{
		auto& fb = MutFramebuffer(fbHandle);

		TextureData depthAttachmentData;
		depthAttachmentData.Width = fb.Width;
		depthAttachmentData.Height = fb.Height;
		depthAttachmentData.Format = format;
		depthAttachmentData.Usage = usage;
		depthAttachmentData.Samples = fb.Samples;

		DeviceTextureHandle depthAttachment = m_texManager.CreateTexture2D(depthAttachmentData);
		fb.DepthStencilAttachment = depthAttachment;
		return depthAttachment;
	}
}
