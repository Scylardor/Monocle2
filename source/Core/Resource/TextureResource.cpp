#include "TextureResource.h"


namespace moe
{
	TextureResource::TextureResource(std::filesystem::path const& imagePath)
	{
		ImageLoader::Data imgData = ImageLoader::Load(imagePath);
		MOE_DEBUG_ASSERT(imgData.IsValid());

		m_textureData.Image = imgData.Image;
		m_textureData.Width = imgData.Width;
		m_textureData.Height = imgData.Height;
		m_textureData.Format = GetFormatForChannelsNumber(imgData.Channels);
	}


	TextureResource::~TextureResource()
	{
		ImageLoader::Free(m_textureData.Image);
	}
}
