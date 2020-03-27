// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Shader/ShaderStage/ShaderStage.h"

#include <string_view>

namespace moe
{

	/**
	 * \brief Describes in a graphics-API agnostic way a give shader module.
	 * A shader module represents a single shader stage (vertex, fragment, geometry).
	 * This descriptor can be used to describe either a source code shader or a binary shader.
	 */
	struct ShaderModuleDescriptor
	{
		/**
		 * \brief The agnostic shader stage of this module
		 */
		ShaderStage	m_moduleStage{ShaderStage::None };


		/**
		 * \brief The actual code of the shader. This can be either source code compiled at runtime or a precompiled binary format.
		 */
		std::string	m_shaderCode{ nullptr };


		/**
		 * \brief Used only for binary code. Will specify the entry point of the shader. If it's null, the string "main" will be used.
		 */
		std::string	m_entryPoint{ nullptr };
	};

}