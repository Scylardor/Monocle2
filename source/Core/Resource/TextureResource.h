#pragma once

#include "BaseResource.h"
#include "Graphics/Texture/TextureFormat.h" // TODO: Core now depends on Graphics... good job ! Need to fix that

#include "Core/Resource/ImageLoader.h"

namespace moe
{

	/* A structure describing the properties of a given image to load from a file.
	 * Mipmaps are useful to tell the graphics RHI what should be the number of mipmaps to generate.
	 */
	struct TextureData
	{
		inline static const uint32_t	MAX_MIPMAPS = (uint32_t)-1;	// As many mipmaps as possible

		unsigned char*					Image{nullptr};
		uint32_t						Width{ 0 };
		uint32_t						Height{ 0 };
		TextureFormat					Format{ TextureFormat::Any };
		uint32_t						Mipmaps{ MAX_MIPMAPS };
	};


	class TextureResource : public IBaseResource
	{
	public:

		TextureResource() = default; // TODO: for retrocompat, remove later.

		TextureResource(std::filesystem::path const& imagePath);

		~TextureResource() override;


	private :

		TextureData	m_textureData{};
	};

}
