// Monocle Game Engine source files

#pragma once

#include "Core/Misc/Types.h"


namespace moe
{
	enum class TextureFormat : unsigned char;

	// TODO: Make that an enum class
	/**
	 * \brief Defines graphics API-agnostic texture usage flag values.
	 * These values indicates how a texture is allowed to be used.
	 */
	enum TextureUsage : uint8_t
	{
		Sampled			= 1 << 0, // The Texture can be used as the target of a read-only texture view, and can be accessed from a shader.
		Storage			= 1 << 1, // The Texture can be used as the target of a read-write texture view, and can be accessed from a shader.
		RenderTarget	= 1 << 2, // The Texture can be used as the color target of a framebuffer
		DepthStencil	= 1 << 3, // The Texture can be used as the depth target of a framebuffer
	};


}