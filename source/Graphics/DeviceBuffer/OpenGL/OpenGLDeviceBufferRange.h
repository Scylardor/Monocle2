// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Core/Misc/Types.h"
#include "Monocle_Graphics_Export.h"

#include <glad/glad.h>

namespace moe
{
	/**
	 * \brief A device buffer range is the same concept as a texture view, but for device buffers.
	 * It allows to reference only part of an existing buffer.
	 * Thus, the device buffer range is made of three parts : the buffer ID, the offset at which the range begins, and the size of the range.
	 */
	struct OpenGLDeviceBufferRange
	{
		GLuint		m_bufferID{0};
		GLintptr	m_offset{0};
		GLsizeiptr	m_size{0};
	};

}

#endif