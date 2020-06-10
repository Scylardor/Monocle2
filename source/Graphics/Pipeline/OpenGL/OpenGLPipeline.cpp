// Monocle Game Engine source files - Alexandre Baron

#include "OpenGLPipeline.h"

#include "glad/glad.h"

namespace moe
{

	void OpenGLPipeline::SetBlendState(const BlendStateDescriptor& bsDesc)
	{
		bsDesc.m_enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);

		GLenum blendSrc = GetOpenGLBlendEnum(bsDesc.m_srcFactor);
		GLenum blendDest = GetOpenGLBlendEnum(bsDesc.m_destFactor);
		glBlendFunc(blendSrc, blendDest);

		GLenum equationMode = GetOpenGLBlendEquation(bsDesc.m_equation);
		glBlendEquation(equationMode);
	}


	int OpenGLPipeline::GetOpenGLBlendEnum(BlendFactor blendFactor)
	{
		switch (blendFactor)
		{
		case BlendFactor::Zero:						return GL_ZERO;
		case BlendFactor::One:						return GL_ONE;
		case BlendFactor::SourceColor:				return GL_SRC_COLOR;
		case BlendFactor::OneMinusSource:			return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::DestColor:				return GL_DST_COLOR;
		case BlendFactor::OneMinusDestColor:		return GL_ONE_MINUS_DST_COLOR;
		case BlendFactor::SourceAlpha:				return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSourceAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DestAlpha:				return GL_DST_ALPHA;
		case BlendFactor::OneMinusDestAlpha:		return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::ConstantColor:			return GL_CONSTANT_COLOR;
		case BlendFactor::OneMinusConstantColor:	return GL_ONE_MINUS_CONSTANT_COLOR;
		case BlendFactor::ConstantAlpha:			return GL_CONSTANT_ALPHA;
		case BlendFactor::OneMinusConstantAlpha:	return GL_ONE_MINUS_CONSTANT_ALPHA;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unmanaged value for OpenGL BlendFactor: %i.", blendFactor);
			return 0;
		}
	}



	int OpenGLPipeline::GetOpenGLBlendEquation(BlendEquation blendEq)
	{
		switch (blendEq)
		{
		case BlendEquation::Add:		return GL_FUNC_ADD;
		case BlendEquation::Sub:		return GL_FUNC_SUBTRACT;
		case BlendEquation::ReverseSub:	return GL_FUNC_REVERSE_SUBTRACT;
		case BlendEquation::Min:		return GL_MIN;
		case BlendEquation::Max:		return GL_MAX;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unmanaged value for OpenGL BlendEquation: %i.", blendEq);
			return 0;
		}
	}


	void OpenGLPipeline::SetDepthStencilState(const DepthStencilStateDescriptor& dsDesc)
	{
		dsDesc.m_depthTest == DepthTest::Enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

		// OpenGL allows us to disable writing to the depth buffer by setting its depth mask to GL_FALSE:
		// Basically, you're (temporarily) using a read-only depth buffer.
		glDepthMask(dsDesc.m_depthWriting == DepthWriting::Enabled);

		GLenum depthFunc = GetOpenGLDSComparisonFunc(dsDesc.m_depthFunc);
		glDepthFunc(depthFunc);

		dsDesc.m_stencilTest == StencilTest::Enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);

		// allows us to set a bitmask that is ANDed with the stencil value about to be written to the buffer.
		// By default this is set to a bitmask of all 1s unaffecting the output, but if we were to set this to 0x00 all the stencil values written to the buffer end up as 0s.
		glStencilMask(dsDesc.m_stencilWriteMask);

		auto [frontSfail, frontDpFail, frontBothPass] = GetOpenGLStencilOps(dsDesc.m_frontFaceOps);
		glStencilOpSeparate(GL_FRONT, frontSfail, frontDpFail, frontBothPass);

		auto[backSfail, backDpFail, backBothPass] = GetOpenGLStencilOps(dsDesc.m_backFaceOps);
		glStencilOpSeparate(GL_BACK, backSfail, backDpFail, backBothPass);

		GLenum frontStencilFunc = GetOpenGLDSComparisonFunc(dsDesc.m_frontFaceOps.m_comparisonFunc);
		glStencilFuncSeparate(GL_FRONT, frontStencilFunc, dsDesc.m_stencilRefVal, dsDesc.m_stencilReadMask);

		GLenum backStencilFunc = GetOpenGLDSComparisonFunc(dsDesc.m_backFaceOps.m_comparisonFunc);
		glStencilFuncSeparate(GL_BACK, backStencilFunc, dsDesc.m_stencilRefVal, dsDesc.m_stencilReadMask);
	}


	void OpenGLPipeline::SetRasterizerState(const RasterizerStateDescriptor& rsDesc)
	{
		if (rsDesc.m_cullMode == CullFace::None)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace(rsDesc.m_cullMode == CullFace::Front ? GL_FRONT : GL_BACK);
		}

		glFrontFace(rsDesc.m_frontFace == FrontFace::Clockwise ? GL_CW : GL_CCW);

		switch (rsDesc.m_polyMode)
		{
			case PolygonMode::Points:		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
			case PolygonMode::Wireframe:	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
			case PolygonMode::Fill:			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
			default:
				MOE_ASSERT(false);
				MOE_ERROR(ChanGraphics, "Unmanaged value for OpenGL PolygonMode: %i.", rsDesc.m_polyMode);
				break;
		}

		// The clipping behavior against the Z position of a vertex can be turned off by activating depth clamping.
		// This is done with glEnable(GL_DEPTH_CLAMP).
		// This will cause the clip - space Z to remain unclipped by the front and rear viewing volume.
		// The Z value computations will proceed as normal through the pipeline.
		// After computing the window-space position, the resulting Z value will be clamped to the glDepthRange (provided by viewport).
		rsDesc.m_depthClip == RasterizerStateDescriptor::Enabled ? glDisable(GL_DEPTH_CLAMP) : glEnable(GL_DEPTH_CLAMP);

		rsDesc.m_scissorTest == RasterizerStateDescriptor::Enabled ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
	}


	int OpenGLPipeline::GetOpenGLPrimitiveTopology(PrimitiveTopology topo)
	{
		switch (topo)
		{
			case PrimitiveTopology::PointList: return GL_POINTS;
			case PrimitiveTopology::LineList:  return GL_LINES;
			case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
			case PrimitiveTopology::TriangleList: return GL_TRIANGLES;
			case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
			default:
				MOE_DEBUG_ASSERT(false);
				MOE_ERROR(moe::ChanGraphics, "Unmanaged topology value: %i", topo);
				return 0;
		}
	}


	int OpenGLPipeline::GetOpenGLDSComparisonFunc(DepthStencilComparisonFunc func)
	{
		switch (func)
		{
		case DepthStencilComparisonFunc::Always:		return GL_ALWAYS;
		case DepthStencilComparisonFunc::Never:			return GL_NEVER;
		case DepthStencilComparisonFunc::Less:			return GL_LESS;
		case DepthStencilComparisonFunc::Equal:			return GL_EQUAL;
		case DepthStencilComparisonFunc::LessEqual:		return GL_LEQUAL;
		case DepthStencilComparisonFunc::Greater:		return GL_GREATER;
		case DepthStencilComparisonFunc::NotEqual:		return GL_NOTEQUAL;
		case DepthStencilComparisonFunc::GreaterEqual:	return GL_GEQUAL;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unmanaged value for OpenGL DepthStencil Comparison Func: %i.", func);
			return 0;
		}
	}


	int OpenGLPipeline::GetOpenGLStencilOpEnum(StencilOp op)
	{
		switch (op)
		{
		case StencilOp::Keep:		return GL_KEEP;
		case StencilOp::Zero:		return GL_ZERO;
		case StencilOp::Replace:	return GL_REPLACE;
		case StencilOp::IncrClamp:	return GL_INCR;
		case StencilOp::IncrWrap:	return GL_INCR_WRAP;
		case StencilOp::DecrClamp:	return GL_DECR;
		case StencilOp::DecrWrap:	return GL_DECR_WRAP;
		case StencilOp::Invert:		return GL_INVERT;
		default:
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Unmanaged value for OpenGL Stencil Op: %i.", op);
			return 0;;
		}
	}


	std::tuple<int, int, int>	OpenGLPipeline::GetOpenGLStencilOps(const StencilOpsDescriptor & op)
	{
		int stencilFailOp = GetOpenGLStencilOpEnum(op.m_stencilFail);
		int depthFailOp = GetOpenGLStencilOpEnum(op.m_depthFail);
		int bothPassOp = GetOpenGLStencilOpEnum(op.m_allPass);

		return { stencilFailOp, depthFailOp, bothPassOp };
	}
}
