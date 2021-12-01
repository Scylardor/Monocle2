// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/moeEnum.h"

namespace moe
{

	/**
	 * \brief A graphics-API agnostic enum describing each possible stage of a shader.
	 */
	MOE_ENUM(ShaderStage, char, // TODO : Make it an enum class because it's causing subtle bugs !
		None = 0,
		Vertex = 1 << 0,
		Fragment = 1 << 1,
		Geometry = 1 << 2,
		TessellationControl = 1 << 3,
		TessellationEvaluation = 1 << 4,
		Compute = 1 << 5
	)

	DECLARE_MOE_ENUM_OPERATORS(ShaderStage);

}