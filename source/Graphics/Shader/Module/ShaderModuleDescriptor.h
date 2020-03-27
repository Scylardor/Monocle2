// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Shader/ShaderStage/ShaderStage.h"

#include <string_view>

namespace moe
{

	struct ShaderModuleDescriptor
	{
		ShaderStage	m_moduleStage{ShaderStage::None };
		std::string	m_shaderCode{ nullptr };
		std::string	m_entryPoint{ nullptr };
	};

}