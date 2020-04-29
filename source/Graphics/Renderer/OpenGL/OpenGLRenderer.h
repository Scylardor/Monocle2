// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/Renderer/AbstractRenderer/AbstractRenderer.h"

#include "Graphics/Shader/Manager/OpenGL/OpenGLShaderManager.h"

#include "Monocle_Graphics_Export.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{

	class OpenGLRenderer final : public AbstractRenderer
	{
	public:

		OpenGLRenderer() = default;
		virtual ~OpenGLRenderer() = default;


		// Delete those functions because OpenGLShaderProgram cannot be copied (avoids duplicate glDeleteProgram's)
		OpenGLRenderer(const OpenGLRenderer&) = delete;

		OpenGLRenderer& operator=(const OpenGLRenderer&) = delete;


		/**
		 * \brief Shutdown method to destroy all renderer-owned OpenGL resources
		 * This was added partially because of an unexplainable crash in a destructor at app exit
		 * because GLAD pointers seem to become NULL before the destruction of OpenGL context.
		 * Shutdown must be called by the user app before the context destruction.
		 */
		Monocle_Graphics_API void	Shutdown() override;


		/**
		 * \brief Creates an OpenGL shader program compiled at runtime, from a descriptor containing GLSL source code.
		 * Functions involved : glShaderSource, glCompileShader
		 * \param shaProDesc a collection of shader module descriptions
		 * \return True if the shader program was successfully created
		 */
		Monocle_Graphics_API ShaderProgramHandle	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) override
		{
			return m_shaderManager.CreateShaderProgramFromSource(shaProDesc);
		}


		/**
		 * \brief Creates a precompiled OpenGL shader program, from a descriptor containing SPIRV binary code.
		 * Functions involved : glShaderBinary, glSpecializeShader
		 * \param shaProDesc a collection of shader module descriptions
		 */
		Monocle_Graphics_API ShaderProgramHandle	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) override
		{
			return m_shaderManager.CreateShaderProgramFromBinary(shaProDesc);
		}


		/**
		 * \brief Removes a shader program from our
		 * \param programHandle
		 * \return
		 */
		Monocle_Graphics_API bool	RemoveShaderProgram(ShaderProgramHandle programHandle) override
		{
			return m_shaderManager.DestroyProgram(programHandle);
		}


		Monocle_Graphics_API [[nodiscard]] VertexLayoutHandle	CreateVertexLayout(const VertexLayoutDescriptor& vertexLayoutDesc) override;


		Monocle_Graphics_API bool	SetupGraphicsContext(GraphicsContextSetup setupFunc) override;

	private:
		OpenGLShaderManager	m_shaderManager;
	};

}

#endif // #ifdef MOE_OPENGL