// Monocle Game Engine source files

#include "OpenGLFramebuffer.h"

#include "Graphics/Texture/OpenGL/OpenGLTextureFormat.h"

#include "glad/glad.h"

namespace moe
{
	void OpenGLFramebuffer::Create()
	{
		glCreateFramebuffers(1, &m_frameBufferID);
	}


	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDescriptor& fbDesc)
	{
		Create();

		if (MOE_ASSERT(m_frameBufferID != 0)) // or something went wrong !
		{
			int colorAttachIdx = 0;
			for (auto& colorAttachment : fbDesc.m_colorAttachments)
			{
				BindAttachment(GL_COLOR_ATTACHMENT0 + colorAttachIdx, colorAttachment);
				colorAttachIdx++;
			}

			BindAttachment(GL_DEPTH_STENCIL_ATTACHMENT, fbDesc.m_depthStencilAttachment);
		}

		MOE_DEBUG_ASSERT(IsComplete());
	}


	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		auto colorTex = m_colorAttachment.Get();
		glDeleteTextures(1, &colorTex);

		auto rbo = m_depthStencilAttachment.Get();
		glDeleteRenderbuffers(1, &rbo);

		glDeleteFramebuffers(1, &m_frameBufferID);
	}


	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
	}


	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	bool OpenGLFramebuffer::IsComplete() const
	{
		bool isComplete = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		return isComplete;
	}


	void OpenGLFramebuffer::BindAttachment(unsigned attachmentID, Texture2DHandle attachmentHandle)
	{
		// Framebuffers can reference either plain textures or renderbuffer objects in OpenGL.
		// I decided to make this distinction "opaque" to the user by hiding it inside the Texture2DHandle.
		// So when binding an attachment, we have to find out if this is a renderbuffer or a texture.
		if (IsARenderBufferHandle(attachmentHandle))
		{
			Texture2DHandle rboHandle = DecodeRenderbufferHandle(attachmentHandle);
			glNamedFramebufferRenderbuffer(m_frameBufferID, attachmentID, GL_RENDERBUFFER, rboHandle.Get());
		}
		else
		{
			// Note that we do not care about any of the wrapping methods or mipmapping since we won't be needing those in most cases.
			glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Used to fix a convolution kernel bug where the sides of the screen suffer from a glitch because of omnidirectional convolution.
			// TODO: make that in a separate sampler !
			glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glNamedFramebufferTexture(m_frameBufferID, attachmentID, attachmentHandle.Get(), 0);
		}
	}
}
