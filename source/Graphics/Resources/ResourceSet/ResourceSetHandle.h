// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	struct Monocle_Graphics_API ResourceSetHandle : RenderableObjectHandle<std::uint32_t>
	{
		static ResourceSetHandle	Null() { return ResourceSetHandle{ 0 }; }

	};

}
