// Monocle Game Engine source files

#include "OpenGLFramebuffer.h"

#include "Graphics/Texture/OpenGL/OpenGLTextureFormat.h"

#include "glad/glad.h"

namespace moe
{
	void OpenGLFramebuffer::Create()
	{
		glCreateFramebuffers(1, &m_frameBufferID);
		MOE_ASSERT(m_frameBufferID < std::numeric_limits<FramebufferHandle::Underlying>::max());
	}


	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDescriptor& fbDesc) :
		m_desc(fbDesc)
	{
		Create();


		if (MOE_ASSERT(m_frameBufferID != 0)) // or something went wrong !
		{
			// Setting up color attachments
			int colorAttachIdx = 0;
			for (auto& colorAttachment : m_desc.m_colorAttachments)
			{
				BindAttachment(GL_COLOR_ATTACHMENT0 + colorAttachIdx, colorAttachment);
				colorAttachIdx++;
			}

			// Setting up depth stencil attachment
			// prefer to use depth/stencil attachment, but if there isn't, use depth
			if (m_desc.m_depthStencilAttachment.IsNotNull())
			{
				BindAttachment(GL_DEPTH_STENCIL_ATTACHMENT, m_desc.m_depthStencilAttachment);
			}
			else if (m_desc.m_depthAttachment.IsNotNull())
			{
				BindAttachment(GL_DEPTH_ATTACHMENT, m_desc.m_depthAttachment);

			}

			// Setting up read/draw buffers if any
			if (m_desc.m_readBuffer != TargetBuffer::Default)
			{
				GLenum targetBuffer = TranslateToOpenGLTargetBufferEnum(m_desc.m_readBuffer);
				glNamedFramebufferReadBuffer(m_frameBufferID, targetBuffer);
			}

			if (m_desc.m_drawBuffer != TargetBuffer::Default)
			{
				GLenum targetBuffer = TranslateToOpenGLTargetBufferEnum(m_desc.m_drawBuffer);
				glNamedFramebufferDrawBuffer(m_frameBufferID, targetBuffer);
			}
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
		bool isComplete = (glCheckNamedFramebufferStatus(m_frameBufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		return isComplete;
	}


	void OpenGLFramebuffer::BindAttachment(unsigned attachmentID, TextureHandle attachmentHandle)
	{
		// Framebuffers can reference either plain textures or renderbuffer objects in OpenGL.
		// I decided to make this distinction "opaque" to the user by hiding it inside the Texture2DHandle.
		// So when binding an attachment, we have to find out if this is a renderbuffer or a texture.
		if (IsARenderBufferHandle(attachmentHandle))
		{
			TextureHandle rboHandle = DecodeRenderbufferHandle(attachmentHandle);
			glNamedFramebufferRenderbuffer(m_frameBufferID, attachmentID, GL_RENDERBUFFER, rboHandle.Get());
		}
		else
		{
			// TODO: remove that, should use a separate sampler.
			// Note that we do not care about any of the wrapping methods or mipmapping since we won't be needing those in most cases.
			glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Used to fix a convolution kernel bug where the sides of the screen suffer from a glitch because of omnidirectional convolution.
			glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(attachmentHandle.Get(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glNamedFramebufferTexture(m_frameBufferID, attachmentID, attachmentHandle.Get(), 0);
		}
	}


	unsigned OpenGLFramebuffer::TranslateToOpenGLTargetBufferEnum(TargetBuffer tgtBuf)
	{
		switch (tgtBuf)
		{
			case TargetBuffer::None: return GL_NONE;
			case TargetBuffer::Back: return GL_BACK;
			default:
				MOE_ASSERT(false);
				MOE_ERROR(ChanGraphics, "Could not translate unmanaged target buffer value: %u", (uint8_t)tgtBuf);
				return 0;
			;
		}
	}
}
