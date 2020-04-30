// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

namespace moe
{

	struct Monocle_Graphics_API MeshHandle : RenderObjectHandle<std::uint32_t>
	{
		static MeshHandle	Null() { return MeshHandle{ 0 }; }

		bool	IsNull() const { return m_handle == 0; }
	};

}
