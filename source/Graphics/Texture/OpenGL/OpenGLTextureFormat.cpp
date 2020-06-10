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
		case TextureFormat::RGBA8:
			return GL_RGBA8;
		case TextureFormat::RGBA32F:
			return GL_RGBA32F;
		case TextureFormat::RGB32F:
			return GL_RGB32F;
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


	Texture2DHandle EncodeRenderbufferHandle(GLuint renderBufferID)
	{
		// Mark the last bit of the handle to notify it's actually a renderbuffer ID
		// TODO: make the texture handle creation code more robust to cover this. It means that we can only have 2^30 textures in flight at the same time, and not 2^31...
		MOE_ASSERT((renderBufferID & (1 << 31)) == false);
		renderBufferID |= (1 << 31);
		return Texture2DHandle{ renderBufferID };
	}

	Texture2DHandle DecodeRenderbufferHandle(Texture2DHandle renderbufferHandle)
	{
		// Just return the handle value with the last bit masked out.
		return Texture2DHandle{ renderbufferHandle.Get() & ~(1 << 31) };
	}


	bool IsARenderBufferHandle(Texture2DHandle texHandle)
	{
		return (texHandle.Get() & (1 << 31));
	}
}
