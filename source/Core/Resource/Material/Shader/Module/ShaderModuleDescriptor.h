// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Resource/Material/Shader/ShaderStage/ShaderStage.h"

#include "Core/Resource/FileResource.h"

#include <string_view>

namespace moe
{

	/**
	 * \brief Describes in a graphics-API agnostic way a give shader module.
	 * A shader module represents a single shader stage (vertex, fragment, geometry).
	 * This Description can be used to describe either a source code shader or a binary shader.
	 */
	struct ShaderModuleDescription
	{
		/**
		 * \brief The agnostic shader stage of this module
		 */
		ShaderStage	m_moduleStage{ShaderStage::Vertex};


		/*
		 * Reference to the shader file that we can read to access the shader code.
		 * Type of file will be useful to know if it's text (needs to be compiled) or binary (SPIR-V).
		 */
		Ref<FileResource>	m_shaderFile{};

		/**
		 * \brief The actual code of the shader. This can be either source code compiled at runtime or a precompiled binary format.
		 */
		std::string	m_shaderCode{ "" };


		/**
		 * \brief Used only for binary code. Will specify the entry point of the shader. If it's null, the string "main" will be used.
		 */
		std::string	m_entryPoint{ "main" };
	};

}