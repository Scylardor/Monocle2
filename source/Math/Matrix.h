// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include "GLM/Matrix_glm.h"

/**
 * \brief A convenience macro to allow me to declare commonly used typedefs
 * \param Dims The dimensions "suffix" of a matrix (e.g. 3x4 to declare Mat3x4)
 * \param ColsN The number of columns of this matrix
 * \param RowsN The number of rows of this matrix
 */
#define DECLARE_MATRIX_USING(Dims, ColsN, RowsN)	\
	using Mat##Dims  = GLM::Matrix<ColsN, RowsN, float>;	\
	using dMat##Dims = GLM::Matrix<ColsN, RowsN, double>;	\
	using iMat##Dims = GLM::Matrix<ColsN, RowsN, int>;		\
	using uMat##Dims = GLM::Matrix<ColsN, RowsN, uint32_t>;	\

namespace moe
{
	DECLARE_MATRIX_USING(3x4, 3, 4);
	DECLARE_MATRIX_USING(3, 3, 3);
	DECLARE_MATRIX_USING(4, 4, 4);
}


#endif
