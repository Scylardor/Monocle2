// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "OpenGLVertexLayout.h"
#include "Graphics/VertexLayout/VertexLayoutHandle.h"

#include <glad/glad.h>

namespace moe
{
	/**
	 * \brief This is a small comparator struct used for the sole purpose of storing VAOs in a set,
	 * but be able to retrieve them with set::find using only the vertex layout handle (we do not want to create new VAOs).
	 */
	struct OpenGLVertexLayoutComparator
	{
		// using C++14 is_transparent to activate Program<->Handle comparison overloads.
		// For more info: https://www.fluentcpp.com/2017/06/09/search-set-another-type-key/
		using is_transparent = void;

		bool operator()(const OpenGLVertexLayout& vao1, const OpenGLVertexLayout& vao2) const
		{
			return (GLuint)vao1 < (GLuint)vao2;
		}


		bool operator()(VertexLayoutHandle handle, const OpenGLVertexLayout & vao) const
		{
			return (GLuint)handle.m_handle < (GLuint)vao;
		}


		bool operator()(const OpenGLVertexLayout & vao, VertexLayoutHandle handle) const
		{
			return (GLuint)vao < (GLuint)handle.m_handle;
		}
	};
}



#endif // MOE_OPENGL