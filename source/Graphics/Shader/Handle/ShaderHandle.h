// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

namespace moe
{

	struct MOE_DLL_API ShaderProgramHandle : RenderObjectHandle<std::uint64_t>
	{
		static ShaderProgramHandle	Null() { return ShaderProgramHandle{ 0 }; }
	};

}
