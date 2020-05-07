// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/Shader/Program/OpenGL/OpenGLShaderProgram.h"
#include "Graphics/Shader/Handle/ShaderHandle.h"

#include <glad/glad.h>

namespace moe
{
	/**
	 * \brief This is a small comparator struct used for the sole purpose of storing shader programs in a set,
	 * but be able to retrieve them with set::find using only the shader ID (we do not want to create new OpenGLShaderProgram objects).
	 */
	struct OpenGLShaderProgramComparator
	{
		// using C++14 is_transparent to activate Program<->Handle comparison overloads.
		// For more info: https://www.fluentcpp.com/2017/06/09/search-set-another-type-key/
		using is_transparent = void;

		bool operator()(const OpenGLShaderProgram& program1, const OpenGLShaderProgram& program2) const
		{
			return (GLuint)program1 < (GLuint)program2;
		}


		bool operator()(ShaderProgramHandle handle, const OpenGLShaderProgram & program) const
		{
			return (GLuint)handle.m_handle < (GLuint)program;
		}


		bool operator()(const OpenGLShaderProgram & program, ShaderProgramHandle handle) const
		{
			return (GLuint)program < (GLuint)handle.m_handle;
		}
	};
}



#endif // MOE_OPENGL