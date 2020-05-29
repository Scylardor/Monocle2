// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{

	/**
	 * \brief A graphics-API agnostic way to describe a viewport.
	 * Viewports are mostly used by cameras to define an area of screen space they render to.
	 * They also control the depth range used by the depth buffer when this viewport is in use.
	 * A range of [0;1] (inclusive) makes full use of the depth buffer.
	 */
	struct ViewportDescriptor
	{
		ViewportDescriptor() = default;

		ViewportDescriptor(float px, float py, float width, float height, float minDepth = 0.f, float maxDepth = 1.f) :
		m_x(px), m_y(py), m_width(width), m_height(height), m_minDepthRange(minDepth), m_maxDepthRange(maxDepth)
		{}

		float	m_x = 0.f;
		float	m_y = 0.f;
		float	m_width = 0.f;
		float	m_height = 0.f;
		float	m_minDepthRange = 0.f;
		float	m_maxDepthRange = 1.f;

		// TODO : Add scissor count and dimensions
		// cf. https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions#page_Viewports-and-scissors
	};

}