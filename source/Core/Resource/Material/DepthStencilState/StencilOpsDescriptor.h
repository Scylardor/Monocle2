// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "DepthStencilComparisonFunc.h"
#include "StencilOps.h"

namespace moe
{

	/**
	 * \brief A graphics API-agnostic struct to describe the stencil operations to use for a given type of face.
	 * It is useful to describe, for example, different stenciling operations based on whether a triangle is a front or back-face.
	 * Each operation describes how a stencil operation should update the stencil buffer.
	 * We can describe the action to take in three cases : if the stencil test fails, if the depth test fails, or if both pass.
	 * The default is to always keep the value in the stencil buffer, which leaves the stencil buffer untouched.
	 * For stenciling effects, you will need to use another construct than the default.
	 */
	struct StencilOpsDescriptor
	{
		StencilOp					m_stencilFail{ StencilOp::Keep };
		StencilOp					m_depthFail{ StencilOp::Keep };
		StencilOp					m_allPass{ StencilOp::Keep };
		DepthStencilComparisonFunc	m_comparisonFunc{ DepthStencilComparisonFunc::Always };
	};


}