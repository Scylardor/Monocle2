// Monocle Game Engine source files - Alexandre Baron

#pragma once


namespace moe
{

	/**
	 * \brief A graphics API-agnostic enum describing a stencil write operation.
	 * It describes how a stencil operation should update the stencil buffer.
	 * We can usually describe the action to take in three cases : if the stencil test fails, if the depth test fails, or if both pass.
	 */
	enum class StencilOp : std::uint8_t
	{
		Keep = 0,	// The currently stored stencil value is kept.
		Zero,		// The stencil value is set to 0.
		Replace,	// The stencil value is replaced with the reference value set with glStencilFunc.
		IncrClamp,	// The stencil value is increased by 1 if it is lower than the maximum value.
		IncrWrap,	// 	Same as GL_INCR, but wraps it back to 0 as soon as the maximum value is exceeded.
		DecrClamp,	// The stencil value is decreased by 1 if it is higher than the minimum value.
		DecrWrap,	// Same as GL_DECR, but wraps it to the maximum value if it ends up lower than 0.
		Invert		// Bitwise inverts the current stencil buffer value.
	};
}