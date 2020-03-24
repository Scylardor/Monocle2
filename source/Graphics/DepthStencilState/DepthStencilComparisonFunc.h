// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{

	/**
	 * \brief A graphics API-agnostic enum describing a depth or stencil test operation.
	 * The depth/stencil test is nothing than a comparison between the candidate fragment value and the value already present in the depth/stencil buffer.
	 * This enum allows to explicitly specify to the API which comparison operation we want to use.
	 * Usually for depth you will want Less, which says if the destination fragment depth (the fragment already on the render target) is less than the current fragment depth,
	 * then draw this new fragment onto the render target. (it means the object is closer to the camera.)
	 * For stencil, it depends on what you want, but using Equal with a reference value of 1 means that all fragments with a stencil value other than 1 will be discarded.
	 */
	enum class DepthStencilComparisonFunc : std::uint8_t
	{
		Always = 0,		// The depth test always passes.
		Never,			// The depth test never passes.
		Less, 			// Passes if the fragment's depth value is less than the stored depth value.
		Equal,	 		// Passes if the fragment's depth value is equal to the stored depth value.
		LessEqual,		// Passes if the fragment's depth value is less than or equal to the stored depth value.
		Greater,		// Passes if the fragment's depth value is greater than the stored depth value.
		NotEqual,		// Passes if the fragment's depth value is not equal to the stored depth value.
		GreaterEqual	// Passes if the fragment's depth value is greater than or equal to the stored depth value.
	};

}