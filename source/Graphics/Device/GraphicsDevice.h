// Monocle Game Engine source files - Alexandre Baron

#pragma once


#include "Graphics/Shader/Handle/ShaderHandle.h"
#include "Graphics/Shader/Program/ShaderProgramDescriptor.h"
#include "Graphics/VertexLayout/OpenGL/OpenGLVertexLayout.h"

#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/VertexLayout/VertexLayoutHandle.h"
#include "Graphics/DeviceBuffer/VertexBufferHandle.h"
#include "Graphics/DeviceBuffer/IndexBufferHandle.h"

#include "Graphics/Camera/ViewportHandle.h"
#include "Graphics/Camera/ViewportDescriptor.h"

namespace moe
{


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

		[[nodiscard]] virtual VertexLayoutHandle	CreateVertexLayout(const VertexLayoutDescriptor& desc) = 0;
		[[nodiscard]] virtual const VertexLayout*	GetVertexLayout(VertexLayoutHandle handle) const = 0;

		[[nodiscard]] virtual VertexBufferHandle	CreateStaticVertexBuffer(const void* data, size_t dataSize) = 0;

		virtual void	DeleteStaticVertexBuffer(VertexBufferHandle vtxHandle) = 0;

		[[nodiscard]] virtual IndexBufferHandle		CreateIndexBuffer(const void* indexData, size_t indexDataSizeBytes) = 0;

		virtual void	DeleteIndexBuffer(IndexBufferHandle idxHandle) = 0;

		[[nodiscard]] virtual ViewportHandle	CreateViewport(const ViewportDescriptor& vpDesc) = 0;
		virtual void	UseViewport(ViewportHandle vpHandle) = 0;

	};
}