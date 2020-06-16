// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Monocle_Graphics_Export.h"

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

namespace moe
{
	struct Monocle_Graphics_API SamplerHandle : RenderObjectHandle<std::uint32_t>
	{
		static SamplerHandle	Null() { return SamplerHandle{ 0 }; }
	};

}
