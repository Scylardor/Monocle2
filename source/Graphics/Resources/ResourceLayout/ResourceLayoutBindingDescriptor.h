// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

#include "ResourceKind.h"

#include "Graphics/Shader/ShaderStage/ShaderStage.h"

namespace moe
{

	/**
	 * \brief Description of a binding belonging to a resource layout.
	 * This is mostly needed to comply to Vulkan API : there is no such thing as a resource layout in OpenGL for example.
	 * This binding descriptors will be bundled in a resource layout descriptor.
	 */
	struct ResourceLayoutBindingDescriptor
	{
		std::string		m_name{""};	// The name of this resource as referenced in the shaders.
		ResourceKind	m_kind;			// The type of this resource (buffer, texture, etc.)
		ShaderStage		m_stageBits;	// Stages this binding descriptor will be used in. This can be OR'ed to indicate multiple stages.
	};

}