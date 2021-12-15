// Monocle Game Engine source files - Alexandre Baron

#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

namespace moe::ImageLoader
{


	Data	Load(std::filesystem::path const& imagePath, int requiredChannels)
	{
		Data imgData;

		if constexpr (std::is_same_v<wchar_t, std::filesystem::path::value_type>)
		{
			static const auto bufLen = 4096; // should be ebough on most systems
			char filebuf[bufLen];
			int err = stbi_convert_wchar_to_utf8(filebuf, bufLen, imagePath.c_str());
			if (!MOE_ASSERT(err != 0))
			{
				if constexpr (moe::IsWindows())
					MOE_ERROR(ChanDefault, "Unable to open file: error: %s\n", moe::GetLastErrorAsString());
				else
					MOE_ERROR(ChanDefault, "Unable to open file\n");
				return {};
			}
			else
			{
				imgData.Image = stbi_load(filebuf, &imgData.Width, &imgData.Height, &imgData.Channels, requiredChannels);
			}
		}
		else
		{
			imgData.Image = stbi_load((const char*)imagePath.c_str(), &imgData.Width, &imgData.Height, &imgData.Channels, requiredChannels);
		}

		MOE_DEBUG_ASSERT(imgData.Image);

		return imgData;
	}

	Data	LoadRadianceHDR(std::filesystem::path const& imagePath, int requiredChannels)
	{
		Data imgData;

		if constexpr (std::is_same_v<wchar_t, std::filesystem::path::value_type>)
		{
			static const auto bufLen = 4096; // should be ebough on most systems
			char filebuf[bufLen];
			int err = stbi_convert_wchar_to_utf8(filebuf, bufLen, imagePath.c_str());
			if (!MOE_ASSERT(err != 0))
			{
				if constexpr (moe::IsWindows())
					MOE_ERROR(ChanDefault, "Unable to open file: error: %s\n", moe::GetLastErrorAsString());
				else
					MOE_ERROR(ChanDefault, "Unable to open file\n");
				return {};
			}
			else
			{
				imgData.Image = (unsigned char*)stbi_loadf(filebuf, &imgData.Width, &imgData.Height, &imgData.Channels, requiredChannels);
			}
		}
		else
		{
			imgData.Image = (unsigned char*) stbi_loadf((const char*)imagePath.c_str(), &imgData.Width, &imgData.Height, &imgData.Channels, requiredChannels);
		}

		MOE_DEBUG_ASSERT(imgData.Image);

		return imgData;
	}

}