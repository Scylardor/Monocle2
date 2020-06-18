// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

#include "Core/Containers/HashMap/HashMap.h"

#include "Graphics/Renderer/AbstractRenderer/AbstractRenderer.h"

#include "Graphics/Device/OpenGL/OpenGLGraphicsDevice.h"

#include "Graphics/Shader/UniformDataKind.h"

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

		[[nodiscard]] virtual VertexLayoutHandle	CreateInstancedVertexLayout(const InstancedVertexLayoutDescriptor& vertexLayoutDesc) override
		{
			return m_device.CreateVertexLayout(vertexLayoutDesc);
		}


		Monocle_Graphics_API [[nodiscard]] DeviceBufferHandle		CreateUniformBuffer(const void* data, uint32_t dataSizeBytes) override;


		Monocle_Graphics_API [[nodiscard]] ResourceLayoutHandle		CreateResourceLayout(const ResourceLayoutDescriptor& desc) override
		{
			return m_device.CreateResourceLayout(desc);
		}

		Monocle_Graphics_API [[nodiscard]] ResourceSetHandle		CreateResourceSet(const ResourceSetDescriptor& desc) override
		{
			return m_device.CreateResourceSet(desc);
		}

		void	UpdateUniformBuffer(DeviceBufferHandle ubHandle, const void* data, size_t dataSizeBytes, uint32_t relativeOffset = 0)
		{
			m_device.UpdateUniformBuffer(ubHandle, data, dataSizeBytes, relativeOffset);
		}


		template <typename T>
		void	UpdateUniformBufferFrom(DeviceBufferHandle ubHandle, const T& data)
		{
			m_device.UpdateUniformBuffer(ubHandle, &data, sizeof(T), 0);
		}


		[[nodiscard]] const IGraphicsDevice&	GetGraphicsDevice() const override final
		{
			return m_device;
		}


		[[nodiscard]] IGraphicsDevice&	MutGraphicsDevice() override
		{
			return m_device;
		}


		UniformResourceKind	GetUniformResourceKind(const std::string& uniformBlockName) const;

		void	SetUniformResourceKind(const std::string& uniformBlockName, UniformResourceKind resourceKind);


		/* TODO : quick and dirty test framework, to remove later */

		Monocle_Graphics_API void	Clear(const ColorRGBAf& clearColor) override;
		Monocle_Graphics_API void	ClearDepth() override;

		Monocle_Graphics_API void	UseMaterial(ShaderProgramHandle progHandle, ResourceSetHandle rscSetHandle) override;

		Monocle_Graphics_API void	UseMaterial(Material* material) override;

		Monocle_Graphics_API void	UseMaterialPerObject(Material* material, AGraphicObject& object) override;

		Monocle_Graphics_API void	UseMaterialInstance(MaterialInstance* material) override;


		Monocle_Graphics_API [[nodiscard]] RenderWorld&	CreateRenderWorld() override;


		Monocle_Graphics_API [[nodiscard]] DeviceBufferHandle	AllocateObjectMemory(const uint32_t size) override final;
		Monocle_Graphics_API void								CopyObjectMemory(DeviceBufferHandle from, uint32_t fromSizeBytes, DeviceBufferHandle to) override final;
		Monocle_Graphics_API void								ReleaseObjectMemory(DeviceBufferHandle freedHandle) override final;

		Monocle_Graphics_API GraphicObjectData					ReallocObjectUniformGraphicData(const GraphicObjectData& oldData, uint32_t newNeededSize) override;

		Monocle_Graphics_API void	UpdateSubBufferRange(DeviceBufferHandle handle, uint32_t offset, void* data, uint32_t dataSize) override;


		Monocle_Graphics_API void	BindFramebuffer(FramebufferHandle fbHandle)  override;
		Monocle_Graphics_API void	UnbindFramebuffer(FramebufferHandle fbHandle) override;

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

		OpenGLGraphicsDevice	m_device;

		OpenGLBuddyAllocator	m_renderWorldMemory;

		HashMap<std::string, UniformResourceKind>	m_uniformResourceKinds;

	};

}

#endif // #ifdef MOE_OPENGL