// Monocle Game Engine source files

#include "OpenGLSampler.h"

#include "glad/glad.h"

namespace moe
{

	unsigned OpenGLSampler::TranslateToOpenGLWrapMode(SamplerWrapping wrapMode)
	{
		switch (wrapMode)
		{
		case SamplerWrapping::Repeat:
			return GL_REPEAT;
		case SamplerWrapping::MirroredRepeat:
			return GL_MIRRORED_REPEAT;
		case SamplerWrapping::ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		case SamplerWrapping::ClampToBorder:
			return GL_CLAMP_TO_BORDER;
		default:
			MOE_DEBUG_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unmanaged wrapping mode value: %u", wrapMode);
			return 0;
		}
	}


	unsigned OpenGLSampler::TranslateToOpenGLFilterMode(SamplerFilter filterMode)
	{
		switch (filterMode)
		{
		case SamplerFilter::Nearest:
			return GL_NEAREST;
		case SamplerFilter::Linear:
			return GL_LINEAR;
		case SamplerFilter::NearestMipmapNearest:
			return GL_NEAREST_MIPMAP_NEAREST;
		case SamplerFilter::LinearMipmapNearest:
			return GL_LINEAR_MIPMAP_NEAREST;
		case SamplerFilter::LinearMipmapLinear:
			return GL_LINEAR_MIPMAP_LINEAR;
		default:
			MOE_DEBUG_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unmanaged filter mode value: %u", filterMode);
			return 0;
		}
	}
}
