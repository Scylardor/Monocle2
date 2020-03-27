// Monocle Game Engine source files - Alexandre Baron

#include "OpenGLShaderStage.h"

namespace moe
{

	GLenum GetShaderStageEnum(ShaderStage stage)
	{
		switch (stage)
		{
		case ShaderStage::Vertex:
			return GL_VERTEX_SHADER;
		case ShaderStage::Fragment:
			return GL_FRAGMENT_SHADER;
		case ShaderStage::Geometry:
			return GL_GEOMETRY_SHADER;
		case ShaderStage::TessellationControl:
			return GL_TESS_CONTROL_SHADER;
		case ShaderStage::TessellationEvaluation:
			return GL_TESS_EVALUATION_SHADER;
		case ShaderStage::Compute:
			return GL_COMPUTE_SHADER;
		default:
			MOE_ERROR(ChanGraphics, "Unmanaged value for shader stage: %d.", +stage);
		}
		return GL_FALSE;
	}
}
