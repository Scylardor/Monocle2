// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include <glm/vec3.hpp>
#include "GLM/Vector.h"

namespace moe
{
	using Vec3 = GLM::Vector<3, float>;
	using dVec3 = GLM::Vector<3, double>;
	using iVec3 = GLM::Vector<3, int>;
	using uVec3 = GLM::Vector<3, uint32_t>;
}


#endif

