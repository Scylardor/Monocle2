// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

namespace moe
{

	struct Monocle_Graphics_API IndexBufferHandle : RenderObjectHandle<std::uint32_t>
	{
		static IndexBufferHandle	Null() { return IndexBufferHandle{ 0 }; }

	};

}
