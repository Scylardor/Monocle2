// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/Shader/ShaderStage/ShaderStage.h"

#include <glad/glad.h>

namespace moe
{

	/**
	 * \brief Translates a Monocle API-agnostic shader stage value in the matching GL enum.
	 * \param stage The Monocle API-agnostic shader stage value
	 * \return A glEnum describing this shader stage or GL_FALSE if this shader stage is None
	 */
	GLenum	GetShaderStageEnum(ShaderStage stage);
}

#endif // MOE_OPENGL