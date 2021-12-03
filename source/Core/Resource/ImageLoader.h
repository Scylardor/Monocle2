#pragma once

#include <STB/stb_image.h>
#include <filesystem>

namespace moe
{
	/* A light wrapper over STB image */
	namespace ImageLoader
	{
		struct Data
		{
			unsigned char*	Image{ nullptr };
			int				Width{ 0 };
			int				Height{ 0 };
			int				Channels{ 4 }; // RGBA by default

			bool	IsValid() const
			{
				return Image != nullptr;
			}
		};

		inline Data	Load(std::filesystem::path const& imagePath, int requiredChannels = 0)
		{
			Data imgData;
			imgData.Image = stbi_load((const char*) imagePath.c_str(), &imgData.Width, &imgData.Height, &imgData.Channels, requiredChannels);
			MOE_DEBUG_ASSERT(imgData.Image);

			return imgData;
		}


		inline Data	LoadRadianceHDR(std::filesystem::path const& imagePath, int requiredChannels = 0)
		{
			Data imgData;
			imgData.Image = (unsigned char *) stbi_loadf((const char*)imagePath.c_str(), &imgData.Width, &imgData.Height, &imgData.Channels, requiredChannels);
			MOE_DEBUG_ASSERT(imgData.Image);

			return imgData;
		}


		inline void	Free(unsigned char* imgData)
		{
			stbi_image_free(imgData);
		}

	}


}