// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <optional>
#endif

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/Renderer/Renderer.h"

#include "Graphics/Device/GraphicsDevice.h"

#include "Graphics/Camera/CameraManager.h"

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

		Monocle_Graphics_API [[nodiscard]] CameraHandle	CreateCamera(const OrthographicCameraDesc& orthoDesc, const ViewportDescriptor& vpDesc) override;
		Monocle_Graphics_API [[nodiscard]] CameraHandle	CreateCamera(const PerspectiveCameraDesc& perspDesc, const ViewportDescriptor& vpDesc) override;

		Monocle_Graphics_API [[nodiscard]] CameraHandle	CreateCamera(const OrthographicCameraDesc& orthoDesc, ViewportHandle vpHandle) override;
		Monocle_Graphics_API [[nodiscard]] CameraHandle	CreateCamera(const PerspectiveCameraDesc& perspDesc, ViewportHandle vpHandle) override;

		[[nodiscard]] const CameraManager&	GetCameraManager() const override	{ return m_cameraManager; }
		[[nodiscard]] CameraManager&		MutCameraManager() override			{ return m_cameraManager; }

	protected:
		[[nodiscard]] virtual const IGraphicsDevice&	GetDevice() const = 0;
		[[nodiscard]] virtual IGraphicsDevice&			MutDevice() = 0;

		CameraManager	m_cameraManager;

	private:

		static std::optional<ShaderProgramDescriptor>	BuildProgramDescriptorFromFileList(const ShaderFileList& fileList);

	};

}