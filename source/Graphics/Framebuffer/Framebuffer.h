// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/RenderTarget/RenderTargetHandle.h"
#include "Graphics/Texture/TextureHandle.h"

namespace moe
{


	/**
	 * \brief A graphics API-agnostic class implementing a Framebuffer.
	 * A framebuffer is basically a holder of render targets you can use for rendering.
	 * It has the notion of "completeness" and using it is valid only it is complete.
	 * The requirements for a valid framebuffer are :
	 * - It needs to have at least one attachment (color, depth or stencil buffer).
	 * - There should be at least one color attachment.
	 * - All attachments should be complete as well (reserved memory).
	 * - Each attachment should have the same number of samples.
	 */
	class AFramebuffer
	{
	public:

		AFramebuffer() = default;

		virtual ~AFramebuffer() = default;

		virtual void	Bind() = 0;
		virtual void	Unbind() = 0;

		virtual void	BindColorAttachment(int colorAttachmentIdx, TextureHandle colorAttachment, int mipLevel = 0, bool layered = false, int layerIdx = 0) = 0;

		virtual void	BindDepthAttachment(TextureHandle depthAttachment, int mipLevel = 0, bool layered = false, int layerIdx = 0) = 0;

		virtual bool	IsComplete() const = 0;

	private:
		Vector<RenderTargetHandle>	m_colorAttachments;
		RenderTargetHandle			m_depthStencilAttachment;
	};

}