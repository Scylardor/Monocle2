// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"


namespace moe
{

	/**
	 * \brief Defines a graphics API-agnostic texture format (number of channels and size in bits of each channel).
	 */
	enum class TextureFormat : uint8_t
	{
		Any,		// Any will do, handy when you don't care basically
		RGBA32F,	// 4 channels using each 32-bit floats. Each component is clamped to the range [0,1].
		RGB32F		// 4 channels using each 32-bit floats Each component is clamped to the range [0,1].
		/* TODO : add more... */
	};


	uint8_t	GetTextureFormatChannelsNumber(TextureFormat format);
}