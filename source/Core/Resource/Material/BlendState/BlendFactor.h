// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{

	/**
	 * \brief A graphics API-agnostic enum describing a transparency blending factor.
	 * It is used during blending computation for transparent objects, and you usually have to provide a factor
	 * for both the source pixel (the new pixel being drawn) and the destination pixel (the pixel already drawn at that position).
	 * Such as the usual blending equation looks something like : FinalColor = (SourceColor * SourceBlendFactor) + (DestinationColor * DestinationBlendFactor)
	 */
	enum class BlendFactor : unsigned char
	{
		Zero = 0,				// Factor is equal to 0
		One,					// Factor is equal to 1
		SourceColor,			// Factor is equal to the source color
		OneMinusSource,			// Factor is equal to 1 - the source color
		DestColor,				// Factor is equal to the destination color
		OneMinusDestColor,		// Factor is equal to 1 - the destination color
		SourceAlpha,			// Factor is equal to the alpha component of the source color
		OneMinusSourceAlpha,	// Factor is equal to 1 - alpha of the source color
		DestAlpha,				// Factor is equal to the alpha component of the destination color
		OneMinusDestAlpha,		// Factor is equal to 1 - alpha of the destination color
		ConstantColor,			// Factor is equal to a constant color
		OneMinusConstantColor,	// Factor is equal to 1 - the constant color
		ConstantAlpha,			// Factor is equal to the alpha component of the constant color
		OneMinusConstantAlpha,	// Factor is equal to 1 - alpha of the constant color vector C¯constant.
	};

}