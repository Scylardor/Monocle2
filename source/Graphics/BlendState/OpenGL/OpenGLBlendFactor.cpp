// Monocle Game Engine source files - Alexandre Baron


#ifdef MOE_OPENGL


#include "OpenGLBlendFactor.h"


GLenum moe::GetOpenGLBlendFactor(BlendFactor factor)
{
	switch (factor)
	{
	case BlendFactor::Zero:
		return GL_ZERO;
	case BlendFactor::One:
		return GL_ONE;
	case BlendFactor::SourceColor:
		return GL_SRC_COLOR;
	case BlendFactor::OneMinusSource:
		return GL_ONE_MINUS_SRC_COLOR;
	case BlendFactor::DestColor:
		return GL_DST_COLOR;
	case BlendFactor::OneMinusDestColor:
		return GL_ONE_MINUS_DST_COLOR;
	case BlendFactor::SourceAlpha:
		return GL_SRC_ALPHA;
	case BlendFactor::OneMinusSourceAlpha:
		return GL_ONE_MINUS_SRC_ALPHA;
	case BlendFactor::DestAlpha:
		return GL_DST_ALPHA;
	case BlendFactor::OneMinusDestAlpha:
		return GL_ONE_MINUS_DST_ALPHA;
	case BlendFactor::ConstantColor:
		return GL_CONSTANT_COLOR;
	case BlendFactor::OneMinusConstantColor:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	case BlendFactor::ConstantAlpha:
		return GL_CONSTANT_ALPHA;
	case BlendFactor::OneMinusConstantAlpha:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	default:
		MOE_ASSERT(false);
		MOE_ERROR(moe::ChanGraphics, "Unknown blend factor value.");
		return GL_ZERO;
	}
}

#endif // #ifdef MOE_OPENGL
