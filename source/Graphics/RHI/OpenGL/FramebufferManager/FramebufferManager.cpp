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

#		if MOE_DEBUG
		static GLint maxColorAttachments = GetMaxColorAttachments();
		bool const isFramebufferFull = fb.ColorAttachments.Size() >= (unsigned)maxColorAttachments;
		MOE_DEBUG_ASSERT(!isFramebufferFull);
		if (isFramebufferFull)
			return DeviceTextureHandle::Null();
#		endif

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


	void OpenGL4FramebufferManager::BindFramebuffer(DeviceFramebufferHandle fbHandle, TargetBuffer /*readBuffer*/, TargetBuffer /*writeBuffer*/)
	{
		auto& fb = MutFramebuffer(fbHandle);
		MOE_DEBUG_ASSERT(fb.FramebufferID != 0);

#		if MOE_DEBUG
			auto status = glCheckNamedFramebufferStatus(fb.FramebufferID, GL_FRAMEBUFFER);
			MOE_DEBUG_ASSERT(status == GL_FRAMEBUFFER_COMPLETE);
#		endif

		glBindFramebuffer(GL_FRAMEBUFFER, fb.FramebufferID);

		int i = 0;
		for (DeviceTextureHandle colorAttachment : fb.ColorAttachments)
		{
			OpenGL4TextureData const& texData = m_texManager.GetTextureData(colorAttachment);
			BindAttachment(fb.FramebufferID, GL_COLOR_ATTACHMENT0 + i, texData.TextureID, texData.Usage);
		}

		if (fb.DepthStencilAttachment.IsNotNull())
		{
			BindDepthStencilAttachment(fb.FramebufferID, fb.DepthStencilAttachment);
		}

	}


	void OpenGL4FramebufferManager::UnbindFramebuffer(DeviceFramebufferHandle /*fbHandle*/)
	{
		UnbindFramebuffer();
	}


	void OpenGL4FramebufferManager::UnbindFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void OpenGL4FramebufferManager::AddColorAttachment(DeviceFramebufferHandle fbHandle, DeviceTextureHandle texHandle)
	{
		auto& fb = MutFramebuffer(fbHandle);
		fb.ColorAttachments.PushBack(texHandle);
	}


	void OpenGL4FramebufferManager::SetDepthStencilAttachment(DeviceFramebufferHandle fbHandle,
		DeviceTextureHandle texHandle)
	{
		auto& fb = MutFramebuffer(fbHandle);
		fb.DepthStencilAttachment = texHandle;
	}

	DeviceTextureHandle OpenGL4FramebufferManager::GetFramebufferColorAttachment(DeviceFramebufferHandle fbHandle, uint32_t colorAttachmentIdx)
	{
		auto const& fb = GetFramebuffer(fbHandle);
		if (!MOE_ASSERT(fb.ColorAttachments.Size() > colorAttachmentIdx))
			return DeviceTextureHandle::Null();

		return fb.ColorAttachments[colorAttachmentIdx];
	}

	DeviceTextureHandle OpenGL4FramebufferManager::GetFramebufferDepthStencilAttachment(DeviceFramebufferHandle fbHandle)
	{
		auto const& fb = GetFramebuffer(fbHandle);
		return fb.DepthStencilAttachment;
	}


	void OpenGL4FramebufferManager::BindAttachment(GLint framebufferID, unsigned attachmentID, GLuint textureID, TextureUsage usage, int mipLevel, bool layered, int layerIdx)
	{
		// Framebuffers can reference either plain textures or renderbuffer objects in OpenGL.
		// I decided to make this distinction "opaque" to the user by hiding it inside the Texture2DHandle.
		// So when binding an attachment, we have to find out if this is a renderbuffer or a texture.
		if ((usage & Sampled) == 0)
		{
			glNamedFramebufferRenderbuffer(framebufferID, attachmentID, GL_RENDERBUFFER, textureID);
		}
		else
		{
			// TODO: remove that, should use a separate sampler.
			// Note that we do not care about any of the wrapping methods or mipmapping since we won't be needing those in most cases.
			//glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//// Used to fix a convolution kernel bug where the sides of the screen suffer from a glitch because of omnidirectional convolution.
			//glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			//glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			if (layered)
			{
				glNamedFramebufferTextureLayer(framebufferID, attachmentID, textureID, mipLevel, layerIdx);
			}
			else
			{
				glNamedFramebufferTexture(framebufferID, attachmentID, textureID, mipLevel);
			}
		}
	}


	void OpenGL4FramebufferManager::BindDepthStencilAttachment(GLint framebufferID, DeviceTextureHandle attachmentHandle)
	{
		OpenGL4TextureData const& texData = m_texManager.GetTextureData(attachmentHandle);

		if (FormatHasStencilComponent(texData.Format))
		{
			BindAttachment(framebufferID, GL_DEPTH_STENCIL_ATTACHMENT, texData.TextureID, texData.Usage);
		}
		else
		{
			BindAttachment(framebufferID, GL_DEPTH_ATTACHMENT, texData.TextureID, texData.Usage);
		}
	}


	GLint OpenGL4FramebufferManager::GetMaxColorAttachments()
	{
		GLint maxColorAttachments{ 0 };
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
		return maxColorAttachments;
	}
}
