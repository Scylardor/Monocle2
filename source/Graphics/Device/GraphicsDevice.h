// Monocle Game Engine source files - Alexandre Baron

#pragma once


#include "Graphics/Shader/Handle/ShaderHandle.h"
#include "Graphics/Shader/Program/ShaderProgramDescriptor.h"
#include "Graphics/VertexLayout/OpenGL/OpenGLVertexLayout.h"

#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/VertexLayout/VertexLayoutHandle.h"
#include "Graphics/DeviceBuffer/VertexBufferHandle.h"
#include "Graphics/DeviceBuffer/IndexBufferHandle.h"
#include "Graphics/DeviceBuffer/UniformBufferHandle.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"
#include "Graphics/Resources/ResourceSet/ResourceSetHandle.h"
#include "Graphics/Resources/ResourceLayout/ResourceLayoutHandle.h"

#include "Graphics/Camera/ViewportHandle.h"
#include "Graphics/Camera/ViewportDescriptor.h"

#include "Graphics/Texture/Texture2DHandle.h"
#include "Graphics/Texture/TextureDescription.h"


namespace moe
{
	class ResourceLayoutDescriptor;
	class ResourceSetDescriptor;


	/**
	 * \brief An interface for an API-agnostic graphics device.
	 * Graphics devices are responsible of creating new graphics resources, allocating GPU memory and managing resource lifetime.
	 * Once the graphics device is destroyed, all the resources it created are destroyed too, and so no one should try to use them after that.
	 */
	class IGraphicsDevice
	{
	public:

		virtual ~IGraphicsDevice() = default;

		virtual void	Initialize() = 0;

		virtual void	Destroy() = 0;

		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) = 0;
		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) = 0;
		virtual bool	RemoveShaderProgram(ShaderProgramHandle programHandle) = 0;

		[[nodiscard]] virtual uint32_t	GetShaderProgramUniformBlockSize(ShaderProgramHandle shaderHandle, const std::string& uniformBlockName) = 0;
		[[nodiscard]] virtual bool	IsPartOfUniformBlock(ShaderProgramHandle shaderHandle, const std::string& uniformBlockName, const std::string& uniformMemberName) const = 0;


		[[nodiscard]] virtual VertexLayoutHandle	CreateVertexLayout(const VertexLayoutDescriptor& desc) = 0;
		[[nodiscard]] virtual const VertexLayout*	GetVertexLayout(VertexLayoutHandle handle) const = 0;

		[[nodiscard]] virtual DeviceBufferHandle	CreateStaticVertexBuffer(const void* data, size_t dataSize) = 0;

		virtual void	DeleteStaticVertexBuffer(DeviceBufferHandle vtxHandle) = 0;

		[[nodiscard]] virtual DeviceBufferHandle		CreateIndexBuffer(const void* indexData, size_t indexDataSizeBytes) = 0;

		virtual void	DeleteIndexBuffer(DeviceBufferHandle idxHandle) = 0;

		virtual void	DrawVertexBuffer(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle, size_t numVertices,
			DeviceBufferHandle idxBufHandle, size_t numIndices) = 0;

		virtual void	UpdateBuffer(DeviceBufferHandle bufferHandle, const void* data, size_t dataSize) const = 0;



		[[nodiscard]] virtual ViewportHandle	CreateViewport(const ViewportDescriptor& vpDesc) = 0;
		virtual void	UseViewport(ViewportHandle vpHandle) = 0;

		[[nodiscard]] virtual DeviceBufferHandle	CreateUniformBuffer(const void* uniformData, size_t uniformDataSizeBytes) = 0;

		[[nodiscard]] virtual ResourceLayoutHandle	CreateResourceLayout(const ResourceLayoutDescriptor& newDesc) = 0;

		[[nodiscard]] virtual ResourceSetHandle		CreateResourceSet(const ResourceSetDescriptor& newDesc) = 0;

		[[nodiscard]] virtual const ResourceLayoutDescriptor&	GetResourceLayoutDescriptor(ResourceLayoutHandle layoutHandle) const = 0;

		[[nodiscard]] virtual const ResourceSetDescriptor&		GetResourceSetDescriptor(ResourceSetHandle setHandle) const = 0;

		[[nodiscard]] virtual Texture2DHandle	CreateTexture2D(const Texture2DDescriptor& tex2DDesc) = 0;
		[[nodiscard]] virtual Texture2DHandle	CreateTexture2D(const Texture2DFileDescriptor& tex2DFileDesc) = 0;

		virtual void	DestroyTexture2D(Texture2DHandle textureHandle) = 0;


	};
}