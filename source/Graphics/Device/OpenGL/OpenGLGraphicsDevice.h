// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#ifdef MOE_STD_SUPPORT
#include <utility> // std::pair
#endif

#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/FreeList/Freelist.h"
#include "Core/Containers/HashMap/HashMap.h"

#include "Graphics/Device/GraphicsDevice.h"

#include "Graphics/VertexLayout/VertexLayoutHandle.h"

#include "Graphics/Shader/Manager/OpenGL/OpenGLShaderManager.h"

#include "Graphics/VertexLayout/OpenGL/OpenGLVertexLayout.h"
#include "Graphics/VertexLayout/OpenGL/OpenGLVertexLayoutComparator.h"

#include "Graphics/GraphicsAllocator/OpenGL/OpenGLBuddyAllocator.h"

#include "Graphics/Camera/ViewportHandle.h"
#include "Graphics/Camera/ViewportDescriptor.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"

#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"

#include "Graphics/Pipeline/PipelineHandle.h"

#include "Graphics/Swapchain/OpenGL/OpenGLSwapchain.h"

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
	class OpenGLGraphicsDevice final : public IGraphicsDevice
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


		virtual void	UpdateUniformBlockVariable(ShaderProgramHandle program, DeviceBufferHandle targetBlockBuffer, const std::string& variableName,
			const void* data, size_t dataSizeBytes) override final;


		GLuint	UseShaderProgram(ShaderProgramHandle programHandle);

		GLuint	GetShaderProgramID(ShaderProgramHandle programHandle);


		Monocle_Graphics_API [[nodiscard]] VertexLayoutHandle	CreateVertexLayout(const VertexLayoutDescriptor& desc) override;
		Monocle_Graphics_API [[nodiscard]] VertexLayoutHandle	CreateVertexLayout(InstancedVertexLayoutDescriptor desc) override; // TODO: remove

		Monocle_Graphics_API [[nodiscard]] const VertexLayout*	GetVertexLayout(VertexLayoutHandle handle) const override;

		const OpenGLVertexLayout*	UseVertexLayout(VertexLayoutHandle layoutHandle);


		Monocle_Graphics_API [[nodiscard]] DeviceBufferHandle	CreateStaticVertexBuffer(const void* data, size_t dataSize) override;

		void	DeleteStaticVertexBuffer(DeviceBufferHandle vtxHandle) override;

		[[nodiscard]] DeviceBufferHandle	CreateIndexBuffer(const void* indexData, size_t indexDataSizeBytes) override;
		void							DeleteIndexBuffer(DeviceBufferHandle idxHandle) override;


		void	DrawVertexBuffer(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle, size_t numVertices,
								DeviceBufferHandle idxBufHandle, size_t numIndices) override;

		void	DrawInstancedMesh(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle, size_t numVertices,
			DeviceBufferHandle idxBufHandle, size_t numIndices, DeviceBufferHandle instancingBuffer, uint32_t instancesAmount) override;

		void	UpdateBuffer(DeviceBufferHandle bufferHandle, const void* data, size_t dataSize) const override;


		[[nodiscard]] ViewportHandle	CreateViewport(const ViewportDescriptor& vpDesc) override;

		void	UseViewport(ViewportHandle vpHandle) override;


		[[nodiscard]] DeviceBufferHandle	CreateUniformBuffer(const void* uniformData, size_t uniformDataSizeBytes) override;

		[[nodiscard]] ResourceLayoutHandle	CreateResourceLayout(const ResourceLayoutDescriptor& newDesc) override;

		[[nodiscard]] ResourceSetHandle		CreateResourceSet(const ResourceSetDescriptor& newDesc) override;


		[[nodiscard]] const ResourceLayoutDescriptor&	GetResourceLayoutDescriptor(ResourceLayoutHandle layoutHandle) const override
		{
			return m_resourceLayouts.Lookup(layoutHandle.Get()-1);
		}

		[[nodiscard]] const ResourceSetDescriptor&		GetResourceSetDescriptor(ResourceSetHandle setHandle) const override
		{
			return m_resourceSets.Lookup(setHandle.Get()-1);
		}


		/**
		 * \brief Creates a texture from a pre-read texture data buffer and known dimensions, etc.
		 * \param tex2DDesc The description of the wanted texture 2D texture data
		 * \return A handle to the created Texture2DHandle or Texture2DHandle::Null if creating the texture failed
		 */
		[[nodiscard]] Texture2DHandle	CreateTexture2D(const Texture2DDescriptor& tex2DDesc) override;


		/**
		 * \brief Creates a texture from a name of a file that the function is going to read for you.
		 * \param tex2DFileDesc The description of the wanted texture 2D texture data
		 * \return A handle to the created Texture2DHandle or Texture2DHandle::Null if creating the texture failed
		 */
		[[nodiscard]] Texture2DHandle	CreateTexture2D(const Texture2DFileDescriptor& tex2DFileDesc) override;


		[[nodiscard]] TextureHandle	CreateCubemapTexture(const CubeMapTextureFilesDescriptor& cubemapFilesDesc) override;


		/**
		 * \brief Destroys a 2D Texture created previously.
		 * \param texHandle The handle of texture to destroy
		 */
		[[nodiscard]] void	DestroyTexture2D(Texture2DHandle texHandle) override;


		[[nodiscard]] uint32_t	GetShaderProgramUniformBlockSize(ShaderProgramHandle shaderHandle, const std::string& uniformBlockName) override;

		[[nodiscard]] bool	IsPartOfUniformBlock(ShaderProgramHandle shaderHandle, const std::string& uniformBlockName, const std::string& uniformMemberName) const override;


		void	BindProgramUniformBlock(GLuint shaderProgramID, const char* uniformBlockName, int uniformBlockBinding, DeviceBufferHandle ubHandle);

		void	BindUniformBlock(unsigned int uniformBlockBinding, DeviceBufferHandle ubHandle, uint32_t bufferSize = 0, uint32_t relativeOffset = 0) override;

		Monocle_Graphics_API void	UpdateUniformBuffer(DeviceBufferHandle ubHandle, const void* data, size_t dataSizeBytes, uint32_t relativeOffset = 0);

		template <typename T>
		void	UpdateUniformBufferFrom(DeviceBufferHandle ubHandle, const T& data)
		{
			UpdateUniformBuffer(ubHandle, &data, sizeof(T));
		}


		void	BindTextureUnitToProgramUniform(GLuint shaderProgramID, int textureUnitIndex, TextureHandle texHandle, const char* uniformName);

		void	BindTextureUnit(int textureBindingPoint, TextureHandle texHandle) override;

		[[nodiscard]]	PipelineHandle	CreatePipeline(PipelineDescriptor& pipelineDesc) override;
		void							SetPipeline(PipelineHandle pipeHandle) override;

		Monocle_Graphics_API	[[nodiscard]]	SwapchainHandle	CreateSwapChain(uint32_t renderWidth, uint32_t renderHeight, FramebufferAttachment wantedAttachments) override;


		Monocle_Graphics_API	[[nodiscard]]	FramebufferHandle	CreateFramebuffer(const FramebufferDescriptor& fbDesc) override;
		[[nodiscard]]							AFramebuffer*		MutFramebuffer(FramebufferHandle fbHandle) override
		{
			return &m_framebuffers[fbHandle.Get()-1];
		}



		static DeviceBufferHandle						EncodeBufferHandle(uint32_t bufferID, uint32_t bufferOffset);

		static std::pair<unsigned int, unsigned int>	DecodeBufferHandle(const RenderObjectHandle<std::uint64_t>& handle);



	private:

		OpenGLBuddyAllocator			m_vertexBufferPool;
		OpenGLBuddyAllocator			m_indexBufferPool;
		OpenGLBuddyAllocator			m_uniformBufferPool;
		HashMap<DeviceBufferHandle, std::uint32_t> m_uniformBufferSizes;

		OpenGLShaderManager				m_shaderManager;

		std::set<OpenGLVertexLayout, OpenGLVertexLayoutComparator>		m_vertexLayouts;

		Freelist<ResourceLayoutDescriptor>	m_resourceLayouts;

		Freelist<ResourceSetDescriptor>		m_resourceSets;

		Vector<GLuint>					m_indexBuffers;

		// Unlike Vulkan or D3D11 for example, OpenGL does not have any concept of "viewport object". So our viewport objects are just descriptors.
		Freelist<ViewportDescriptor>	m_viewports;

		// Same for pipelines - doesn't exist in OpenGL. Just keep the descriptors.
		Freelist<PipelineDescriptor>	m_pipelines;

		OpenGLSwapchain	m_swapChain;

		Vector<OpenGLFramebuffer>	m_framebuffers; // TODO: this is a code smell. Should be a free list but needs to fix compile...

		GLenum	m_primitiveTopology = GL_TRIANGLES;	// Current topology used to draw geometry. Modified by SetPipeline
	};
}

#endif