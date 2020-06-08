// Monocle Game Engine source files

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "Graphics/Texture/Texture2DHandle.h"

namespace moe
{

	/**
	 * \brief A graphics APi-agnostic descriptor of what textures, or render targets, should be used to populate a framebuffer.
	 */
	struct FramebufferDescriptor
	{
		FramebufferDescriptor() = default;

		Vector<Texture2DHandle>	m_colorAttachments;
		Texture2DHandle			m_depthStencilAttachment;
	};

}