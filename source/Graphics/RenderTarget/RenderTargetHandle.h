// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	struct Monocle_Graphics_API RenderTargetHandle : RenderObjectHandle<std::uint32_t>
	{
		static RenderTargetHandle	Null() { return RenderTargetHandle{ 0 }; }

	};

}
