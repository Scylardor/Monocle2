#include "BlendStateDescriptor.h"

namespace moe
{
	const BlendStateDescriptor BlendStateDescriptor::ms_Override(BlendFactor::One, BlendFactor::Zero, BlendEquation::Add, BlendStateDescriptor::Enabled);
	const BlendStateDescriptor BlendStateDescriptor::ms_Additive(BlendFactor::One, BlendFactor::One, BlendEquation::Add, BlendStateDescriptor::Enabled);
	const BlendStateDescriptor BlendStateDescriptor::ms_AdditiveAlpha(BlendFactor::SourceAlpha, BlendFactor::One, BlendEquation::Add, BlendStateDescriptor::Enabled);
	const BlendStateDescriptor BlendStateDescriptor::ms_BlendAlpha(BlendFactor::SourceAlpha, BlendFactor::OneMinusSourceAlpha, BlendEquation::Add, BlendStateDescriptor::Enabled);
	const BlendStateDescriptor BlendStateDescriptor::ms_Disabled(BlendFactor::SourceAlpha, BlendFactor::OneMinusSourceAlpha, BlendEquation::Add, BlendStateDescriptor::Disabled);

}