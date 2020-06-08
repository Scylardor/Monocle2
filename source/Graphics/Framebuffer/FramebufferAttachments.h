// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Monocle_Graphics_Export.h"

namespace moe
{

	/**
	 * \brief A bitflag combination describing which attachments should be present in a framebuffer, and their data format.
	 */
	enum FramebufferAttachment
	{
		ColorAttachment_Rgb8	= 1 << 0,	// RGB components with 8 bits per component
		DepthStencil_24_8		= 1 << 1	// Depth-stencil attachment with 32 bits shared for 24-bit depth precision and 8-bit stencil precision
	};

}