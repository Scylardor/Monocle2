// Monocle Game Engine source files - Alexandre Baron

#pragma once


#include "Graphics/Shader/Handle/ShaderHandle.h"
#include "Graphics/Shader/Program/ShaderProgramDescriptor.h"

#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/VertexLayout/VertexLayoutHandle.h"

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

		virtual void	Destroy() = 0;

		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) = 0;
		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) = 0;
		virtual bool	RemoveShaderProgram(ShaderProgramHandle programHandle) = 0;

		[[nodiscard]] virtual VertexLayoutHandle	CreateVertexLayout(const VertexLayoutDescriptor& desc) = 0;
	};
}