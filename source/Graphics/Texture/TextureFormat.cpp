// Monocle Game Engine source files - Alexandre Baron

#include "TextureFormat.h"


uint8_t moe::GetTextureFormatChannelsNumber(TextureFormat format)
{
	switch (format)
	{
	case TextureFormat::Any:
		return 0;
	case TextureFormat::RGBA32F:
		return 4;
	case TextureFormat::RGB32F:
		return 3;
	default:
		MOE_ASSERT(false);
		MOE_ERROR(ChanGraphics, "Could not read unmanaged texture format value.");
		return 0;
	}
}
