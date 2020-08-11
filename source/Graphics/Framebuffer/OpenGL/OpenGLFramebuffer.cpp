// Monocle Game Engine source files

#include "OpenGLFramebuffer.h"

#include "Graphics/Texture/OpenGL/OpenGLTextureFormat.h"

#include "Graphics/Framebuffer/FramebufferHandle.h"

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
			SetupColorAttachments();

			SetupDepthStencilAttachment();

			SetupReadBuffer();

			SetupDrawBuffers();
		}

		if (fbDesc.m_doCompletenessCheck == CompleteCheck::Enabled)
			MOE_DEBUG_ASSERT(IsComplete());
	}


	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		auto colorTex = m_colorAttachment.Get();
		glDeleteTextures(1, &colorTex); // TODO: this could be a render buffer instead ? Should fix this texture/renderbuffer interface!

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


	void OpenGLFramebuffer::BindColorAttachment(int colorAttachmentIdx, TextureHandle colorAttachment, int mipLevel, bool layered, int layerIdx)
	{
		BindAttachment(GL_COLOR_ATTACHMENT0 + colorAttachmentIdx, colorAttachment, mipLevel, layered, layerIdx);
	}


	void OpenGLFramebuffer::BindDepthAttachment(TextureHandle depthAttachment, int mipLevel, bool layered, int layerIdx)
	{
		BindAttachment(GL_DEPTH_ATTACHMENT, depthAttachment, mipLevel, layered, layerIdx);
	}


	bool OpenGLFramebuffer::IsComplete() const
	{
		auto status = glCheckNamedFramebufferStatus(m_frameBufferID, GL_FRAMEBUFFER);
		bool isComplete = (status == GL_FRAMEBUFFER_COMPLETE);
		return isComplete;
	}


	void OpenGLFramebuffer::SetupColorAttachments()
	{
#		if MOE_DEBUG
		GLint maxColorAttachments = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
		MOE_DEBUG_ASSERT(m_desc.m_colorAttachments.Size() < (unsigned)maxColorAttachments);
#		endif

		int colorAttachIdx = 0;
		for (auto& colorAttachment : m_desc.m_colorAttachments)
		{
			BindAttachment(GL_COLOR_ATTACHMENT0 + colorAttachIdx, colorAttachment);
			colorAttachIdx++;
		}
	}


	void OpenGLFramebuffer::SetupDepthStencilAttachment()
	{
		// prefer to use depth/stencil attachment, but if there isn't, use depth
		if (m_desc.m_depthStencilAttachment.IsNotNull())
		{
			BindAttachment(GL_DEPTH_STENCIL_ATTACHMENT, m_desc.m_depthStencilAttachment);
		}
		else if (m_desc.m_depthAttachment.IsNotNull())
		{
			BindAttachment(GL_DEPTH_ATTACHMENT, m_desc.m_depthAttachment);
		}
	}


	void OpenGLFramebuffer::SetupReadBuffer()
	{
		if (m_desc.m_readBuffer != TargetBuffer::Default)
		{
			GLenum targetBuffer = TranslateToOpenGLTargetBufferEnum(m_desc.m_readBuffer);
			glNamedFramebufferReadBuffer(m_frameBufferID, targetBuffer);
		}
	}


	void OpenGLFramebuffer::SetupDrawBuffers()
	{
		switch (m_desc.m_drawBuffer)
		{
		case TargetBuffer::AllColorAttachments:
			{
				// Bind all color attachments
				// TODO: I'd like it to be vector on stack !
				Vector<GLenum> attachments(m_desc.m_colorAttachments.Size());
				for (int iAttach = 0; iAttach < m_desc.m_colorAttachments.Size(); ++iAttach)
				{
					attachments[iAttach] = GL_COLOR_ATTACHMENT0 + iAttach;
				}

				glNamedFramebufferDrawBuffers(m_frameBufferID, (GLsizei) attachments.Size(), attachments.Data());
			}
			break;
		case TargetBuffer::Default: // nothing to do
			break;
		default: // any other value
			GLenum targetBuffer = TranslateToOpenGLTargetBufferEnum(m_desc.m_drawBuffer);
			glNamedFramebufferDrawBuffer(m_frameBufferID, targetBuffer);
			break;
		}
	}


	void OpenGLFramebuffer::BindAttachment(unsigned attachmentID, TextureHandle attachmentHandle, int mipLevel, bool layered, int layerIdx)
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

			if (layered)
			{
				glNamedFramebufferTextureLayer(m_frameBufferID, attachmentID, attachmentHandle.Get(), mipLevel, layerIdx);
			}
			else
			{
				glNamedFramebufferTexture(m_frameBufferID, attachmentID, attachmentHandle.Get(), mipLevel);
			}
		}

	}


	unsigned OpenGLFramebuffer::TranslateToOpenGLTargetBufferEnum(TargetBuffer tgtBuf)
	{
		switch (tgtBuf)
		{
			case TargetBuffer::None: return GL_NONE;
			default:
				MOE_ERROR(ChanGraphics, "Could not translate unmanaged target buffer value: %u", (uint8_t)tgtBuf);
				MOE_ASSERT(false);
				return 0;
		}
	}
}
