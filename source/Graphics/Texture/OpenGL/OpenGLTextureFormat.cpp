// Monocle Game Engine source files - Alexandre Baron

#include "OpenGLTextureFormat.h"

namespace moe
{
	GLenum TranslateToOpenGLSizedFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::Any:
		case TextureFormat::RGBA8:
			return GL_RGBA8;
		case TextureFormat::SRGB_RGBA8:
			return GL_SRGB8_ALPHA8;
		case TextureFormat::RGBA16F:
			return GL_RGBA16F;
		case TextureFormat::RGBA32F:
			return GL_RGBA32F;
		case TextureFormat::R8:
			return GL_R8;
		case TextureFormat::R32F:
			return GL_R32F;
		case TextureFormat::RG16F:
			return GL_RG16F;
		case TextureFormat::RGB8:
			return GL_RGB8;
		case TextureFormat::SRGB_RGB8:
			return GL_SRGB8;
		case TextureFormat::RGB32F:
			return GL_RGB32F;
		case TextureFormat::RGBE:
		case TextureFormat::RGB16F:
			return GL_RGB16F;
		case TextureFormat::Depth16:
			return GL_DEPTH_COMPONENT16;
		case TextureFormat::Depth24:
			return GL_DEPTH_COMPONENT24;
		case TextureFormat::Depth32:
			return GL_DEPTH_COMPONENT32;
		case TextureFormat::Depth32F:
			return GL_DEPTH_COMPONENT32F;
		case TextureFormat::Depth24_Stencil8:
			return GL_DEPTH24_STENCIL8;
		case TextureFormat::Depth32F_Stencil8:
			return GL_DEPTH32F_STENCIL8;
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
		case TextureFormat::RGBA32F:
			return GL_RGBA;
		case TextureFormat::RGB32F:
		case TextureFormat::RGB16F:
		case TextureFormat::RGB8:
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


	GLenum TranslateToOpenGLTypeEnum(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::RGBA8:
			case TextureFormat::SRGB_RGBA8:
			case TextureFormat::RGB8:
			case TextureFormat::SRGB_RGB8:
				return GL_UNSIGNED_BYTE;
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
			case TextureFormat::RGB32F:
				return GL_FLOAT;
			default:
				MOE_ERROR(ChanGraphics, "Unmanaged value given to TranslateToOpenGLTypeEnum.");
				MOE_DEBUG_ASSERT(false);
		}

		return 0;
	}


	TextureHandle EncodeRenderbufferHandle(GLuint renderBufferID)
	{
		// Mark the last bit of the handle to notify it's actually a renderbuffer ID
		// TODO: make the texture handle creation code more robust to cover this. It means that we can only have 2^30 textures in flight at the same time, and not 2^31...
		MOE_ASSERT((renderBufferID & (1 << 31)) == false);
		renderBufferID |= (1 << 31);
		return TextureHandle{ renderBufferID };
	}

	TextureHandle DecodeRenderbufferHandle(TextureHandle renderbufferHandle)
	{
		// Just return the handle value with the last bit masked out.
		return TextureHandle{ renderbufferHandle.Get() & ~(1 << 31) };
	}


	bool IsARenderBufferHandle(TextureHandle texHandle)
	{
		return (texHandle.Get() & (1 << 31));
	}
}
