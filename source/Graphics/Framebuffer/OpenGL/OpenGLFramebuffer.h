// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Framebuffer/Framebuffer.h"
#include "Graphics/Framebuffer/FramebufferDescription.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{

	/**
	 * \brief OpenGL implementation of a framebuffer.
	 * It is a light encapsulation over a OpenGL framebuffer.
	 * Allows to bind render targets to attachments and to query its completeness.
	 * This interface does not allow blitting, as of now.
	 */
	class OpenGLFramebuffer final : public AFramebuffer
	{
	public:
		OpenGLFramebuffer() = default;
		OpenGLFramebuffer(const FramebufferDescriptor& fbDesc);

		Monocle_Graphics_API ~OpenGLFramebuffer();

		OpenGLFramebuffer(const OpenGLFramebuffer& other) = delete;
		OpenGLFramebuffer& operator=(const OpenGLFramebuffer& rhs) = delete;

		OpenGLFramebuffer(OpenGLFramebuffer&& other)
		{
			m_frameBufferID = other.m_frameBufferID;
			other.m_frameBufferID = 0;
		}

		OpenGLFramebuffer& operator=(OpenGLFramebuffer&& rhs) noexcept
		{
			if (&rhs != this)
			{
				m_frameBufferID = rhs.m_frameBufferID;
				rhs.m_frameBufferID = 0;
			}

			return *this;
		}

		Monocle_Graphics_API void	Create();

		void	Bind() override;

		void	Unbind() override;

		bool	IsComplete() const override;

		unsigned int	GetID() const { return m_frameBufferID; }


	private:

		void	SetupColorAttachments();

		void	SetupDepthStencilAttachment();

		void	SetupReadBuffer();

		void	SetupDrawBuffers();

		void	BindAttachment(unsigned int attachmentID, TextureHandle attachmentHandle);

		static	unsigned int	TranslateToOpenGLTargetBufferEnum(TargetBuffer tgtBuf);

		unsigned int		m_frameBufferID{0};

		RenderTargetHandle	m_colorAttachment{0};
		RenderTargetHandle	m_depthStencilAttachment{0};

		FramebufferDescriptor	m_desc;
	};

}