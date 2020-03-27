// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/BlendState/BlendEquation.h"

#include <glad/glad.h>

namespace moe
{

	/**
	 * \brief Translates an agnostic Monocle BlendEquation enum value into an OpenGL blend equation value
	 * \param factor The Monocle blend equation value to translate
	 * \return A GLenum containing the blend equation value understandable by OpenGL
	 */
	GLenum	GetOpenGLBlendEquation(BlendEquation blEq);

}

#endif // #ifdef MOE_OPENGL