// Monocle Game Engine source files - Alexandre Baron

#pragma once


#include "Graphics/BlendState/BlendStateDescriptor.h"
#include "Graphics/DepthStencilState/DepthStencilDescriptor.h"

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
	};

}