// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <optional>
#endif

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/Renderer/Renderer.h"

#include "Graphics/Device/GraphicsDevice.h"

#include "Graphics/RenderWorld/RenderWorld.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	/**
	 * \brief Intermediate abstract Renderer class that provides some useful general-purpose functions implementations.
	 * This is the class you usually want to inherit from rather than the pure interface.
	 */
	class AbstractRenderer : public IGraphicsRenderer
	{
	public:
		using ShaderFileList = Vector<std::pair<ShaderStage, std::string_view>>;

		virtual ~AbstractRenderer() = default;


		Monocle_Graphics_API [[nodiscard]] ShaderProgramHandle	CreateShaderProgramFromSourceFiles(const ShaderFileList& fileList) final override;

		Monocle_Graphics_API [[nodiscard]] ShaderProgramHandle	CreateShaderProgramFromBinaryFiles(const ShaderFileList& fileList) final override;


		Monocle_Graphics_API [[nodiscard]] RenderWorld&	CreateRenderWorld() override
		{
			auto renderWorldID = m_renderWorlds.Add(*this);
			return m_renderWorlds.Lookup(renderWorldID);
		}


		Monocle_Graphics_API void	UseResourceSet(const ResourceSetHandle rscSetHandle) override;



	protected:
		[[nodiscard]] virtual const IGraphicsDevice&	GetDevice() const = 0;
		[[nodiscard]] virtual IGraphicsDevice&			MutDevice() = 0;



	private:

		Freelist<RenderWorld>	m_renderWorlds;

		static std::optional<ShaderProgramDescriptor>	BuildProgramDescriptorFromFileList(const ShaderFileList& fileList);

	};

}