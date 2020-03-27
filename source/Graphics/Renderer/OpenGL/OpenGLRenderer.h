// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include <Graphics/Renderer/Renderer.h>

#include <glad/glad.h>

namespace moe
{

	class OpenGLRenderer : public IGraphicsRenderer
	{
	public:


		/**
		 * \brief Creates an OpenGL shader program compiled at runtime, from a descriptor containing GLSL source code.
		 * Functions involved : glShaderSource, glCompileShader
		 * \param shaProDesc a collection of shader module descriptions
		 */
		void	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) override;


		/**
		 * \brief Creates a precompiled OpenGL shader program, from a descriptor containing SPIRV binary code.
		 * Functions involved : glShaderBinary, glSpecializeShader
		 * \param shaProDesc a collection of shader module descriptions
		 */
		void	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) override;
	};

}

#endif // #ifdef MOE_OPENGL