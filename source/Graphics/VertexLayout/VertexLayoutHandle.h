// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

namespace moe
{

	struct Monocle_Graphics_API VertexLayoutHandle : RenderObjectHandle<std::uint32_t>
	{
		static VertexLayoutHandle	Null() { return VertexLayoutHandle{ 0 }; }

		bool	IsNull() const { return m_handle == 0; }
	};

}
