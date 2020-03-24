// Monocle Game Engine source files - Alexandre Baron

#pragma once


#include "Graphics/BlendState/BlendFactor.h"

#include <glad/glad.h>

namespace moe
{

	/**
	 * \brief Translates an agnostic Monocle BlendFactor enum value into an OpenGL blend factor value
	 * \param factor The Monocle blend factor value to translate
	 * \return A GLenum containing the blend factor value understandable by OpenGL
	 */
	GLenum	GetOpenGLBlendFactor(BlendFactor factor);


}