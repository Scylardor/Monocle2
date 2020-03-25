// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{

	/**
	 * \brief A graphics API-agnostic enum to describe primitive topology.
	 * This describes the way the rasterizer is to interpret geometry data.
	 */
	enum class PrimitiveTopology
	{
		PointList = 0,	// A series of isolated points.
		LineList,		// A series of isolated, 2-element line segments.
		LineStrip,		// A series of connected line segments.
		TriangleList,	// A list of isolated, 3-element triangles.
		TriangleStrip	// A series of connected triangles.
	};


}