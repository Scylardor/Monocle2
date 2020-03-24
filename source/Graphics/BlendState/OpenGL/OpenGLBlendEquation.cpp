// Monocle Game Engine source files - Alexandre Baron

#include "OpenGLBlendEquation.h"


GLenum	moe::GetOpenGLBlendEquation(BlendEquation blEq)
{
	switch (blEq)
	{
	case BlendEquation::Add:
		return GL_FUNC_ADD;
	case BlendEquation::Sub:
		return GL_FUNC_SUBTRACT;
	case BlendEquation::ReverseSub:
			return GL_FUNC_REVERSE_SUBTRACT;
	case BlendEquation::Min:
		return GL_MIN;
	case BlendEquation::Max:
		return GL_MAX;
	default:
		MOE_ASSERT(false);
		MOE_ERROR(moe::ChanGraphics, "Unknown blend equation value.");
		return GL_FUNC_ADD;
	}
}
