// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "DepthStencilComparisonFunc.h"

#include "StencilOpsDescriptor.h"

#include "Core/Misc/Types.h"

namespace moe
{


	/**
	 * \brief Describes in a graphics API-agnostic way everything needed to know how to configure depth and stencil testing.
	 */
	struct DepthStencilStateDescriptor
	{
		/**
		* \brief Controls whether depth-stencil settings are enabled or not (I find it more expressive than a bool)
		*/
		enum State : char
		{
			Disabled = 0,
			Enabled
		};

		State						m_depthTest{ Disabled };

		State						m_depthWriting{ Enabled };


		/**
		 * \brief Describes how should we compare a new fragment's depth value to the value stored in the depth buffer.
		 * If this comparison returns true, the new value replaces the old one in the buffer.
		 * Usually set to less because in clip space, smaller z means we're closer to the camera. (so it is kept if it's LESS than the depth buffer value)
		 */
		DepthStencilComparisonFunc	m_depthFunc{ DepthStencilComparisonFunc::Less };

		State			m_stencilTest{ Disabled };


		/**
		 * \brief The mask used to bitwise-AND the reference value and the stored stencil value before the stencil test compares them.
		 */
		std::uint8_t	m_stencilReadMask{ 0xFF };


		/**
		 * \brief The mask used to bitwise-AND a stencil value about to be written to the buffer.
		 * Usually set to all 1s, unaffecting the output, but setting it to 0x00 equates disabling stencil writing.
		 */
		std::uint8_t	m_stencilWriteMask{ 0xFF };


		/**
		 * \brief The integer reference value that is used in the stencil comparison. It is clamped to the range [0,2^n - 1], where n is the number of bitplanes in the stencil buffer.
		 * Usually set to 0
		 */
		std::uint32_t	m_stencilRefVal{ 0 };


		/**
		 * \brief Stencil operations to use for triangles that are front faces
		 */
		StencilOpsDescriptor	m_frontFaceOps;


		/**
		 * \brief Stencil operations to use for triangles that are back faces
		 */
		StencilOpsDescriptor	m_backFaceOps;


		/**
		* \brief This blend state will keep depth/stencil disabled. It is the default state.
		*/
		static const DepthStencilStateDescriptor	ms_Disabled;
	};
}