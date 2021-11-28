// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Resource/Material/BlendState/BlendStateDescriptor.h"
#include "Core/Resource/Material/DepthStencilState/DepthStencilStateDescriptor.h"
#include "Core/Resource/Material/RasterizerState/RasterizerStateDescriptor.h"
#include "Core/Resource/Material/Topology/PrimitiveTopology.h"

namespace moe
{

	/**
	 * \brief This describes how a graphics pipeline should be built.
	 * It is meant to describe a pipeline object in an agnostic way (regardless of OpenGL, D3D, Vulkan or Metal).
	 */
	struct PipelineDescriptor
	{
		BlendStateDescriptor		m_blendStateDesc;
		DepthStencilStateDescriptor	m_depthStencilStateDesc;
		RasterizerStateDescriptor	m_rasterizerStateDesc;
		PrimitiveTopology			m_topology{ PrimitiveTopology::TriangleList };
	};

}