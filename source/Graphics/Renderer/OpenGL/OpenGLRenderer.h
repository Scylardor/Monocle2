// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/Renderer/AbstractRenderer/AbstractRenderer.h"

#include "Graphics/Device/OpenGL/OpenGLGraphicsDevice.h"

#include "Graphics/Mesh/Mesh.h"

#include "Core/Containers/FreeList/Freelist.h"

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
		 * \brief Renderer initialization method
		 */
		Monocle_Graphics_API bool	Initialize(IGraphicsRenderer::GraphicsContextSetup setupFunction) override;

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
			return m_device.CreateShaderProgramFromSource(shaProDesc);
		}


		/**
		 * \brief Creates a precompiled OpenGL shader program, from a descriptor containing SPIRV binary code.
		 * Functions involved : glShaderBinary, glSpecializeShader
		 * \param shaProDesc a collection of shader module descriptions
		 */
		Monocle_Graphics_API ShaderProgramHandle	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) override
		{
			return m_device.CreateShaderProgramFromBinary(shaProDesc);
		}


		/**
		 * \brief Removes a shader program from our graphics device
		 * \param programHandle
		 * \return
		 */
		Monocle_Graphics_API bool	RemoveShaderProgram(ShaderProgramHandle programHandle) override
		{
			return m_device.RemoveShaderProgram(programHandle);
		}


		Monocle_Graphics_API [[nodiscard]] VertexLayoutHandle	CreateVertexLayout(const VertexLayoutDescriptor& vertexLayoutDesc) override
		{
			return m_device.CreateVertexLayout(vertexLayoutDesc);
		}

		Monocle_Graphics_API [[nodiscard]] MeshHandle	CreateStaticMeshFromBuffer(const MeshDataDescriptor& vertexData, const MeshDataDescriptor& indexData) override;

		Monocle_Graphics_API void	DeleteStaticMesh(MeshHandle handle) override;


		Monocle_Graphics_API void	UseCamera(CameraHandle camHandle) override;

		Monocle_Graphics_API void	Clear(const ColorRGBAf& clearColor) override;

		Monocle_Graphics_API void	UseMaterial(ShaderProgramHandle progHandle) override;

		Monocle_Graphics_API void	DrawMesh(MeshHandle meshHandle, VertexLayoutHandle layoutHandle) override;

	protected:

		[[nodiscard]] const IGraphicsDevice&	GetDevice() const override
		{
			return m_device;
		}

		[[nodiscard]] IGraphicsDevice&	MutDevice() override
		{
			return m_device;
		}

	private:

		static	void	OpenGLDebugMessageRoutine(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

		Mesh&	MutMesh(MeshHandle handle)
		{
			return m_meshFreelist.Lookup(handle.Get() - 1); // - 1 to get back the original index !
		}

		OpenGLGraphicsDevice	m_device;

		Freelist<Mesh>	m_meshFreelist;


	};

}

#endif // #ifdef MOE_OPENGL