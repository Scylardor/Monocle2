// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Monocle_Graphics_Export.h"

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

namespace moe
{

	struct Monocle_Graphics_API ShaderProgramHandle : RenderableObjectHandle<std::uint64_t>
	{
		static ShaderProgramHandle	Null() { return ShaderProgramHandle{ 0 }; }
	};

}
