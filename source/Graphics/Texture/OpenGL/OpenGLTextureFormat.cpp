// Monocle Game Engine source files - Alexandre Baron

#include "OpenGLTextureFormat.h"

namespace moe
{
	GLenum TranslateToOpenGLSizedFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::Any:
			return GL_RGBA32F;
		case TextureFormat::RGBA32F:
			return GL_RGBA32F;
		case TextureFormat::RGB32F:
			return GL_RGB32F;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Could not translate unmanaged texture format value.");
			return 0;
		}
	}


	GLenum TranslateToOpenGLBaseFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::Any:
			return GL_RGBA;
		case TextureFormat::RGBA32F:
			return GL_RGBA;
		case TextureFormat::RGB32F:
			return GL_RGB;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Could not translate unmanaged texture format value.");
			return 0;
		}
	}

	GLenum TranslateToOpenGLBaseFormat(int channelsNbr)
	{
		switch (channelsNbr)
		{
		case 1:
			return GL_RED;
		case 2:
			return GL_RG;
		case 3:
			return GL_RGB;
		case 4:
			return GL_RGBA;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Could not translate unmanaged texture format value.");
			return 0;
		}
	}
}
