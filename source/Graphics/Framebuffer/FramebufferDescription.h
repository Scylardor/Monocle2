// Monocle Game Engine source files

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "Graphics/Texture/Texture2DHandle.h"


namespace moe
{

	/**
	 * \brief Graphics-API agnostic to describe a type of target buffer.
	 * Useful to specify which buffer is to be used for read and draw operations.
	 */
	enum class TargetBuffer : uint8_t
	{
		None = 0,
		Default = 1, // Uses only first color attachment.
		AllColorAttachments = 2, // Uses all specified color attachments.
		Color = 4,
		Depth = 8,
		Stencil = 16
		// Could add more...
	};

	/* Used to specify if you want completeness check on framebuffer creation.
	 * You can skip it if you create the framebuffer before having all attachments.
	 * But then you're on your own to provide a complete framebuffer (no safety net).
	 */
	enum class CompleteCheck
	{
		Enabled,
		Disabled
	};

	/**
	 * \brief A graphics APi-agnostic descriptor of what textures, or render targets, should be used to populate a framebuffer.
	 */
	struct FramebufferDescriptor
	{
		FramebufferDescriptor() = default;

		Vector<Texture2DHandle>	m_colorAttachments;
		TextureHandle			m_depthAttachment;
		Texture2DHandle			m_depthStencilAttachment;

		// The source color buffer to use for all read operations when this framebuffer is bound.
		TargetBuffer			m_readBuffer{ TargetBuffer::Default };

		// The destination color buffer to use for all write operations when this framebuffer is bound.
		TargetBuffer			m_drawBuffer{ TargetBuffer::Default };

		CompleteCheck			m_doCompletenessCheck{CompleteCheck::Enabled};

	};

}