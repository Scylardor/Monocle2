// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Resource/Material/Shader/ShaderStage/ShaderStage.h"

#include "Core/Resource/FileResource.h"


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
		ShaderStage	Stage{ShaderStage::Vertex};


		/*
		 * Reference to the shader file that we can read to access the shader code.
		 * Type of file will be useful to know if it's text (needs to be compiled) or binary (SPIR-V).
		 */
		Ref<FileResource>	ShaderFile{};

		/**
		 * \brief The actual code of the shader. This can be either source code compiled at runtime or a precompiled binary format.
		 */
		std::string	ShaderCode{ "" };


		/**
		 * \brief Used only for binary code. Will specify the entry point of the shader. If it's null, the string "main" will be used.
		 */
		std::string	EntryPoint{ "main" };
	};

}