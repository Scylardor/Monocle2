// Monocle Game Engine source files - Alexandre Baron

#include "TextureFormat.h"


uint8_t moe::GetTextureFormatChannelsNumber(TextureFormat format)
{
	switch (format)
	{
	case TextureFormat::Depth16:
	case TextureFormat::Depth24:
	case TextureFormat::Depth32:
	case TextureFormat::Depth32F:
	case TextureFormat::Depth24_Stencil8:
	case TextureFormat::Depth32F_Stencil8:
	case TextureFormat::RG8:
	case TextureFormat::RG16F:
		return 1;
	case TextureFormat::R8:
	case TextureFormat::R32F:
		return 2;
	case TextureFormat::RGB8:
	case TextureFormat::RGB32F:
	case TextureFormat::SRGB_RGB8:
	case TextureFormat::RGB16F:
	case TextureFormat::RGBE:
		return 3;
	case TextureFormat::Any:
	case TextureFormat::RGBA32F:
	case TextureFormat::RGBA8:
	case TextureFormat::RGBA16F:
	case TextureFormat::SRGB_RGBA8:
		return 4;
	default:
		MOE_ASSERT(false);
		MOE_ERROR(ChanGraphics, "Could not read unmanaged texture format value.");
		return 0;

	}
}
