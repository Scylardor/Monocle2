#pragma once


#ifdef MOE_WINDOWS
// I want to be able to convert wchar_t to char for filenames.
#define STBI_WINDOWS_UTF8
#include "Core/Misc/Windows/GetLastErrorAsString.h"
#endif

#include <STB/stb_image.h>
#include <filesystem>

#include "Core/Misc/moeSystem.h"

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

		Data	Load(std::filesystem::path const& imagePath, int requiredChannels = 0);

		Data	LoadRadianceHDR(std::filesystem::path const& imagePath, int requiredChannels = 0);

		inline void	VerticallyFlipTexturesOnLoad(bool enabled)
		{
			stbi_set_flip_vertically_on_load(enabled);
		}

		inline void	Free(unsigned char* imgData)
		{
			stbi_image_free(imgData);
		}

	}


}