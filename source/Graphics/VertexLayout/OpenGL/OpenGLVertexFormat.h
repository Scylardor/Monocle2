// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/VertexLayout/VertexElementFormat.h"

#include "Core/Misc/Types.h"

#include <optional>

#include <glad/glad.h>

namespace moe
{

	struct OpenGLVertexElementFormat
	{
		static std::optional<OpenGLVertexElementFormat>	TranslateFormat(VertexElementFormat vtxFormat);
		static std::optional<uint32_t>	FindTypeSize(std::uint8_t numCpnts, GLenum type);

		GLenum			m_type{ GL_FALSE };
		std::uint8_t	m_numCpnts{ 0 };
		GLboolean		m_normalized{ false };
	};

}

#endif