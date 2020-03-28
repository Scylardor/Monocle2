// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/Renderer/AbstractRenderer/AbstractRenderer.h"

#include "Graphics/Shader/Manager/OpenGL/OpenGLShaderManager.h"

#include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{

	class MOE_DLL_API OpenGLRenderer final : public AbstractRenderer
	{
	public:


		/**
		 * \brief Creates an OpenGL shader program compiled at runtime, from a descriptor containing GLSL source code.
		 * Functions involved : glShaderSource, glCompileShader
		 * \param shaProDesc a collection of shader module descriptions
		 * \return True if the shader program was successfully created
		 */
		ShaderProgramHandle	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) override
		{
			return m_shaderManager.CreateShaderProgramFromSource(shaProDesc);
		}


		/**
		 * \brief Creates a precompiled OpenGL shader program, from a descriptor containing SPIRV binary code.
		 * Functions involved : glShaderBinary, glSpecializeShader
		 * \param shaProDesc a collection of shader module descriptions
		 */
		ShaderProgramHandle	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) override
		{
			return m_shaderManager.CreateShaderProgramFromBinary(shaProDesc);
		}


		bool	RemoveShaderProgram(ShaderProgramHandle programHandle) override
		{
			return m_shaderManager.DestroyProgram(programHandle);
		}


	private:
		OpenGLShaderManager	m_shaderManager;
	};

}

#endif // #ifdef MOE_OPENGL