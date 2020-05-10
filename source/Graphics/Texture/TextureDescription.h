// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

#include "Core/Misc/Literals.h"

#include "Core/Misc/Types.h"

#include "TextureFormat.h"

namespace moe
{
	struct Texture2DDescriptor
	{
		char*			m_imageData{ nullptr };
		Width_t			m_width{1};
		Height_t		m_height{1};
		TextureFormat	m_targetFormat{ TextureFormat::Any };
		uint32_t		m_wantedMipmapLevels{ 4 };
	};

	struct Texture2DFileDescriptor
	{
		std::string		m_filename;
		TextureFormat	m_targetFormat{ TextureFormat::Any };
		TextureFormat	m_requiredFormat{ TextureFormat::Any };
		uint32_t		m_wantedMipmapLevels{4};
	};
}