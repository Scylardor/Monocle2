// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{

	/**
	 * \brief A graphics API-agnostic enum describing a transparency blending equation.
	 * It is used during blending computation for transparent objects to determine which color the final pixel should have.
	 * The usual blending equation looks something like : FinalColor = (SourceColor * SourceBlendFactor) + (DestinationColor * DestinationBlendFactor)
	 * So most of the time you will want Add (+), but other operations exist.
	 */
	enum class BlendEquation
	{
		Add = 0,	// the default, adds both colors to each other, result = Src + Dst
		Sub,		// subtracts both colors from each other,  result = Src - Dst
		ReverseSub,	// subtracts both colors, but reverses order : result = Dst - Src
		Min,		// takes the component-wise minimum of both colors : result = min(Dst, Src)
		Max			// takes the component-wise maximum of both colors : result = max(Dst, Src)
	};

}