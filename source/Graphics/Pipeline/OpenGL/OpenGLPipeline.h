// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Pipeline/PipelineDescriptor.h"

namespace moe
{
	class OpenGLPipeline
	{
	public:
		static void	SetBlendState(const BlendStateDescriptor& bsDesc);

		static void	SetDepthStencilState(const DepthStencilStateDescriptor& dsDesc);

		static void	SetRasterizerState(const RasterizerStateDescriptor& rsDesc);

		static int	GetOpenGLPrimitiveTopology(PrimitiveTopology topo);

		static int	GetOpenGLBlendEnum(BlendFactor blendFactor);

		static int	GetOpenGLBlendEquation(BlendEquation blendEq);

		static int	GetOpenGLDSComparisonFunc(DepthStencilComparisonFunc func);

		static std::tuple<int, int, int>	GetOpenGLStencilOps(const StencilOpsDescriptor& ops);

		static int	GetOpenGLStencilOpEnum(StencilOp op);
	private:

	};

}
