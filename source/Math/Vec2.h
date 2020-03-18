// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM
#include "GLM/Vector.h"

namespace moe
{
	using Vec2 = GLM::Vector<2, float>;
	using dVec2 = GLM::Vector<2, double>;
	using iVec2 = GLM::Vector<2, int>;
	using uVec2 = GLM::Vector<2, uint32_t>;
}


#endif

