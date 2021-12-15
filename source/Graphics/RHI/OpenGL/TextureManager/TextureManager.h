#pragma once

#include <glad/glad.h>

#include "Graphics/RHI/TextureManager/TextureManager.h"

#include "Core/Resource/TextureResource.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{


	struct OpenGL4TextureData : TextureData
	{
		OpenGL4TextureData(TextureData const& data, GLuint texID) :
			TextureData(data),
			TextureID(texID)
		{}

		GLuint	TextureID{0};
	};


	/*
	 * An abstract class for textures (1D, 2D, 3D, cubemap ...) and render targets (sometimes AKA renderbuffers) management.
	 */
	class OpenGL4TextureManager : public ITextureManager
	{
	public:

		~OpenGL4TextureManager() override = default;

		[[nodiscard]] DeviceTextureHandle Monocle_Graphics_API	CreateTexture2DFromFile(Ref<TextureResource> const& textureFile) override;

		[[nodiscard]] DeviceTextureHandle Monocle_Graphics_API	CreateTexture2D(TextureData const& data) override;

		void	FreeUnderlyingTexture2D(DeviceTextureHandle tex);
		void	ResizeTexture2D(DeviceTextureHandle tex, std::pair<int, int> const& newDimensions);
		void	DestroyTexture2D(DeviceTextureHandle tex);

		[[nodiscard]] OpenGL4TextureData const&	GetTextureData(DeviceTextureHandle handle) const
		{
			return m_textures.Get((ObjectPoolID) handle.Get());
		}

	private:
		void	FreeUnderlyingTexture2D(OpenGL4TextureData& texData);

		DynamicObjectPool<OpenGL4TextureData>	m_textures;

	};



}
