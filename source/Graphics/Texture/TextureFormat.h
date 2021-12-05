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
		R8,					//	1 channel using a 8-bit unsigned integer. Each component is clamped to the range [0,1].
		RG8,				//	2 channels using a 8-bit unsigned integer. Each component is clamped to the range [0,1].
		RG16F,				//	2 channels using each 16-bit floats. Each component is clamped to the range [0,1].
		RGBA8,				//	4 channels using each 8-bit unsigned integer. Each component is clamped to the range [0,1].
		SRGB_RGBA8,			//	4 channels using each 8-bit unsigned integer. Each component is clamped to the range [0,1]. SRGB version (to be used for gamma-corrected images)
		RGBA16F,			//	4 channels using each 16-bit floats. Each component is clamped to the range [0,1].
		RGBA32F,			//	4 channels using each 32-bit floats. Each component is clamped to the range [0,1].
		RGB8,				//	3 channels using each 8-bit unsigned integer. Each component is clamped to the range [0,1].
		SRGB_RGB8,			//	3 channels using each 8-bit unsigned integer. Each component is clamped to the range [0,1]. SRGB version (to be used for gamma-corrected images)
		R32F,				//	1 channel using a 8-bit unsigned integer. Each component is clamped to the range [0,1].
		RGB32F,				//	3 channels using each 32-bit floats. Each component is clamped to the range [0,1].
		RGB16F,				//	3 channels using each 16-bit floats. Each component is clamped to the range [0,1].
		RGBE,				//	Radiance HDR format. It stores pixels as one byte each for RGB (red, green, and blue) values with a one byte shared exponent. Thus it stores four bytes per pixel.
		Depth16,			//	Can store any 16-bit normalized integer value of depth information. It maps the integer range onto the depth values [0,1].
		Depth24,			//	Can store any 24-bit normalized integer value of depth information. It maps the integer range onto the depth values [0,1].
		Depth32,			//	Can store any 32-bit normalized integer value of depth information. It maps the integer range onto the depth values [0,1].
		Depth32F,			//	Can store any 32-bit floating-point value of depth information.
		Depth24_Stencil8,	//	Combined depth/stencil format (24 bits of depth and 8 bits of stencil).
		Depth32F_Stencil8	//	Combined depth/stencil format (32 bits of depth and 8 bits of stencil).
		/* TODO : add more... */
	};


	uint8_t	GetTextureFormatChannelsNumber(TextureFormat format);

	inline TextureFormat	GetFormatForChannelsNumber(int channelsNbr)
	{
		switch (channelsNbr)
		{
		case 1:
			return TextureFormat::R8;
		case 2:
			return TextureFormat::RG8;
		case 3:
			return TextureFormat::RGB8;
		case 4:
			return TextureFormat::RGBA8;
		default:
			MOE_ASSERT(false); // unmanaged value
			return TextureFormat::RGBA8;
		}
	}

	inline bool	FormatHasStencilComponent(TextureFormat format)
	{
		return (format == TextureFormat::Depth24_Stencil8 || format == TextureFormat::Depth32F_Stencil8);
	}


	inline TextureFormat	GetRadianceHDRFormat()						{ return TextureFormat::RGBE; }
	inline bool				IsRadianceHDRFormat(TextureFormat format)	{ return (format == TextureFormat::RGBE); }
}