#pragma once

#include "BlendFactor.h"
#include "BlendEquation.h"

#include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{

	/**
	 * \brief Describes how the blending state of the pipeline should be set up.
	 * It is an API-agnostic way of describing which blending factors and blending equation we want to use.
	 * It also includes a blending constant color for APIs that support it.
	 * The usual blending equation looks something like : FinalColor = (SourceColor * SourceBlendFactor) + (DestinationColor * DestinationBlendFactor).
	 * Also includes often used presets made easy to use (BlendAlpha is the most usual formula, but Additive is also be popular).
	 */
	struct MOE_DLL_API BlendStateDescriptor
	{

		/**
		 * \brief Controls whether blending is enabled or not (I find it more expressive than a bool)
		 */
		enum State : char
		{
			Disabled = 0,
			Enabled
		};


		BlendStateDescriptor() :
		BlendStateDescriptor(ms_Disabled)
		{}

		BlendStateDescriptor(BlendFactor srcFactor, BlendFactor dstFactor, BlendEquation blendEq, State enabled = State::Disabled) :
			m_srcFactor(srcFactor), m_destFactor(dstFactor), m_enabled(enabled)
		{}


		BlendFactor	m_srcFactor{ BlendFactor::SourceAlpha };
		BlendFactor	m_destFactor{ BlendFactor::OneMinusSourceAlpha };
		State		m_enabled{ State::Disabled };

		/**
		 * \brief This Blend State always overrides the destination pixel with the source pixel color.
		 */
		static const BlendStateDescriptor	ms_Override;


		/**
		 * \brief This blend state will give you a blend equation where output_color = 1 * source_color + 1 * destination_color.
		 */
		static const BlendStateDescriptor	ms_Additive;


		/**
		 * \brief This blend state will give you a blend equation where output_color = source_alpha * source_color + 1 * destination_color.
		 * It can be a good tradeoff when you don't want to have to sort by depth. With additive alpha blending, the order objects are drawn doesn't matter.
		 * Addition is commutative: no matter what colors you have, you're always going to get the same result.
		 * You just have to disable depth writing.
		 * See for example https://docs.microsoft.com/en-us/archive/blogs/etayrien/alpha-blending-part-3
		 */
		static const BlendStateDescriptor	ms_AdditiveAlpha;

		/**
		* \brief This blend state will give you a blend equation where output_color = source_alpha * source_color + (1 - source_alpha) * destination_color.
		* This is the "most correct" way to do transparency, but it comes at a cost :
		* you must deal with disabling depth writing (to make sure transparent objects do not obscure each other because of depth test)
		* and you must sort transparent objects back-to-front to ensure the computed color is right.
		*/
		static const BlendStateDescriptor	ms_BlendAlpha;


		/**
		 * \brief This blend state will keep blending disabled.
		 */
		static const BlendStateDescriptor	ms_Disabled;
	};

}