#pragma once

#include "Graphics/RHI/TextureManager/TextureManager.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{


	/*
	 * An abstract class for textures (1D, 2D, 3D, cubemap ...) and render targets (sometimes AKA renderbuffers) management.
	 */
	class OpenGL4TextureManager : public ITextureManager
	{
	public:

		~OpenGL4TextureManager() override = default;

		DeviceTextureHandle Monocle_Graphics_API	CreateTexture2DFromFile(Ref<FileResource> const& textureFile) override;

		DeviceTextureHandle Monocle_Graphics_API	CreateTexture2D() override;
	};



}
