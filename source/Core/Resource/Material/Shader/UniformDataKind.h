// Monocle Game Engine source files - Alexandre Baron

#pragma once


namespace moe
{

	/**
	 * \brief This enum describes how often an uniform block should be updated:
	 * In least to most frequently used order:
	 * - each time a camera is used to draw
	 * - each time a material is bound
	 * - each time an object is drawn.
	 */
	enum class UniformResourceKind : std::uint8_t
	{
		None = 0,
		PerCamera,
		PerMaterial,
		PerObject
	};

}