// Monocle Game Engine source files

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	struct Monocle_Graphics_API TextureHandle : RenderObjectHandle<std::uint32_t>
	{
		static TextureHandle	Null() { return TextureHandle{ 0 }; }

	};
}