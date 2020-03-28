// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <utility> // std::pair
#include <string_view>
#endif

#include "Core/Containers/Vector/Vector.h"
#include "Graphics/Shader/Handle/ShaderHandle.h"
#include "Graphics/Shader/Program/ShaderProgramDescriptor.h"


namespace moe
{
	struct ShaderProgramDescriptor;

	/**
	 * \brief The base class for all other renderer classes.
	 *	Contains the graphics device necessary to output commands to the GPU.
	 *	Acts like a Facade to the rest of the world, hiding the underlying HAL rendering system.
	 *	https://en.wikipedia.org/wiki/Facade_pattern
	 */
	class IGraphicsRenderer
	{
	public:

		using ShaderFileList = Vector<std::pair<ShaderStage, std::string_view>>;

		virtual ~IGraphicsRenderer() = default;

		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) = 0;
		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) = 0;

		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromSourceFiles(const ShaderFileList& fileList) = 0;
		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromBinaryFiles(const ShaderFileList& fileList) = 0;

		virtual bool				RemoveShaderProgram(ShaderProgramHandle programHandle) = 0;

	};

}