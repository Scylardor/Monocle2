// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"
#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{

	struct Monocle_Graphics_API FramebufferHandle : RenderObjectHandle<std::uint16_t>
	{
		static FramebufferHandle	Null() { return FramebufferHandle{ 0 }; }

	};
}
