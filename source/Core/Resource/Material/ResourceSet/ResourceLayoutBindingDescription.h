// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

#include "BindingType.h"


#include "Core/Resource/Material/Shader/ShaderStage/ShaderStage.h"

namespace moe
{

	/**
	 * \brief Description of a binding belonging to a resource layout.
	 * This is mostly needed to comply to Vulkan API : there is no such thing as a resource layout in OpenGL for example.
	 * This binding descriptors will be bundled in a resource layout descriptor.
	 */
	struct ResourceLayoutBindingDescription
	{
		ResourceLayoutBindingDescription() = default;


		ResourceLayoutBindingDescription(std::string name, uint16_t bindingPoint, BindingType type, ShaderStage stage) :
			Name(std::move(name)), BindingPoint(bindingPoint), Type(type), Stages(stage)
		{}

		std::string		Name{""};					// The name of this resource as referenced in the shaders.
		std::uint16_t	BindingPoint{0};			// The number of the binding within a resource set.
		BindingType		Type{ BindingType::None};	// The type of this resource (buffer, texture, etc.)
		ShaderStage		Stages{ShaderStage::None};	// Stages this binding descriptor will be used in. This can be OR'ed to indicate multiple stages.
	};

}