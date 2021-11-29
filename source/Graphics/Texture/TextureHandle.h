// Monocle Game Engine source files

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	struct Monocle_Graphics_API TextureHandle : RenderableObjectHandle<std::uint32_t>
	{
		static TextureHandle	Null() { return TextureHandle{ 0 }; }

	};

	// Renamed to not break backwards compatibility
	struct Monocle_Graphics_API TexHandle
	{
		TexHandle() = default;
		TexHandle(std::uint32_t handle, std::uint32_t renderObjID) :
			 Value(handle), RenderObjectID(renderObjID)
		{}

		std::uint32_t	Value{0};

		std::uint32_t	RenderObjectID{ ms_INVALID_ID };

		static TexHandle	Null() { return TexHandle{ ms_INVALID_ID, 0 }; }

	private:
		static const std::uint32_t ms_INVALID_ID = (std::uint32_t) -1;

	};
}