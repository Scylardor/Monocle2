// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <optional>
#endif

#include "Core/Containers/Vector/Vector.h"
#include "Monocle_Graphics_Export.h"

#include "Graphics/Renderer/Renderer.h"


namespace moe
{

	/**
	 * \brief Intermediate abstract Renderer class that provides some useful general-purpose functions implementations.
	 * This is the class you usually want to inherit from rather than the pure interface.
	 */
	class Monocle_Graphics_API AbstractRenderer : public IGraphicsRenderer
	{
	public:
		using ShaderFileList = Vector<std::pair<ShaderStage, std::string_view>>;

		virtual ~AbstractRenderer() = default;


		[[nodiscard]] ShaderProgramHandle	CreateShaderProgramFromSourceFiles(const ShaderFileList& fileList) final override;

		[[nodiscard]] ShaderProgramHandle	CreateShaderProgramFromBinaryFiles(const ShaderFileList& fileList) final override;

	private:
		static std::optional<ShaderProgramDescriptor>	BuildProgramDescriptorFromFileList(const ShaderFileList& fileList);

	};

}