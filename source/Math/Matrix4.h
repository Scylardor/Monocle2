// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include <glm/mat4x4.hpp>
#include "GLM/Matrix.h"

namespace moe
{
	using Mat4  = GLM::Matrix<4, 4, float>;
	using dMat4 = GLM::Matrix<4, 4, double>;
	using iMat4 = GLM::Matrix<4, 4, int>;
	using uMat4 = GLM::Matrix<4, 4, uint32_t>;
}


#endif
