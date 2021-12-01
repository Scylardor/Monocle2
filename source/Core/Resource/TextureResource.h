#pragma once

#include "BaseResource.h"
#include "Graphics/Texture/TextureFormat.h"

#include <filesystem>

namespace moe
{

	/* A structure describing the properties of a given image to load from a file.
	 * Mipmaps are useful to tell the graphics RHI what should be the number of mipmaps to generate.
	 */
	struct ImageData
	{
		std::string				Image{};
		uint32_t				Width{ 0 };
		uint32_t				Height{ 0 };
		TextureFormat			Format{ TextureFormat::Any };
		uint32_t				Mipmaps{ 1 };
	};


	class TextureResource : public IBaseResource
	{
	public:

		TextureResource() = default; // TODO: for retrocompat, remove later.

		TextureResource(std::filesystem::path const& imagePath);

		~TextureResource() override = default;


	private :

		ImageData	m_imageData{};
	};

}
