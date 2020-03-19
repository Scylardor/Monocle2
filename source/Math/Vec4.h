// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include <glm/vec4.hpp>
#include "GLM/Vector_glm.h"

namespace moe
{
	using Vec4  = GLM::Vector<4, float>;
	using dVec4 = GLM::Vector<4, double>;
	using iVec4 = GLM::Vector<4, int>;
	using uVec4 = GLM::Vector<4, uint32_t>;
}


#endif

