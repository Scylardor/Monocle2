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
		Any,				//	Any will do, handy when you don't care basically
		RGBA32F,			//	4 channels using each 32-bit floats. Each component is clamped to the range [0,1].
		RGB32F,				//	4 channels using each 32-bit floats. Each component is clamped to the range [0,1].
		Depth32,			//	Can store any 32-bit normalized integer value of depth information. It maps the integer range onto the depth values [0,1].
		Depth32F,			//	Can store any 32-bit floating-point value of depth information.
		Depth24_Stencil8,	//	Combined depth/stencil format (24 bits of depth and 8 bits of stencil).
		Depth32F_Stencil8	//	Combined depth/stencil format (32 bits of depth and 8 bits of stencil).
		/* TODO : add more... */
	};


	uint8_t	GetTextureFormatChannelsNumber(TextureFormat format);
}