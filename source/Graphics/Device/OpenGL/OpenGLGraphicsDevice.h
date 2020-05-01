// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/Device/GraphicsDevice.h"

#include "Graphics/VertexLayout/VertexLayoutHandle.h"

#include "Graphics/Shader/Manager/OpenGL/OpenGLShaderManager.h"

#include "Graphics/VertexLayout/OpenGL/OpenGLVertexLayout.h"

#include "Graphics/GraphicsAllocator/OpenGL/OpenGLBuddyAllocator.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{
	/**
	 * \brief OpenGL implementation of the graphics device.
	 * May be the "weirdest" abstraction of the graphics engine, as OpenGL being a global state machine,
	 * it does not make a lot of sense to create an object that pretends to manage OpenGL resources
	 * (there is no way to enforce correct usage of resources or ensuring their lifetime).
	 * So it's more of a convenient wrapper than anything else.
	 */
	class OpenGLGraphicsDevice : public IGraphicsDevice
	{

	public:

		Monocle_Graphics_API OpenGLGraphicsDevice() = default;

		Monocle_Graphics_API void	Initialize() override;

		Monocle_Graphics_API void	Destroy() override;


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
		 * \brief Removes a shader program from our graphics device
		 * \param programHandle
		 * \return
		 */
		Monocle_Graphics_API bool	RemoveShaderProgram(ShaderProgramHandle programHandle) override
		{
			return m_shaderManager.DestroyProgram(programHandle);
		}


		Monocle_Graphics_API [[nodiscard]] VertexLayoutHandle	CreateVertexLayout(const VertexLayoutDescriptor& desc) override;

		Monocle_Graphics_API [[nodiscard]] const VertexLayout*	GetVertexLayout(VertexLayoutHandle handle) const override;


		Monocle_Graphics_API [[nodiscard]] VertexBufferHandle	CreateStaticVertexBuffer(const void* data, size_t dataSize) override;
		void	DeleteStaticVertexBuffer(VertexBufferHandle vtxHandle) override;

		[[nodiscard]] IndexBufferHandle	CreateIndexBuffer(const void* indexData, size_t indexDataSizeBytes) override;

		virtual void	DeleteIndexBuffer(IndexBufferHandle idxHandle) override;

	private:

		OpenGLBuddyAllocator		m_vertexBufferPool;
		OpenGLBuddyAllocator		m_indexBufferPool;

		OpenGLShaderManager			m_shaderManager;

		Vector<OpenGLVertexLayout>	m_layouts;

		Vector<GLuint>				m_indexBuffers;
	};
}

#endif