// Monocle Game Engine source files

#pragma once

#include "Graphics/Texture/TextureHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	struct Monocle_Graphics_API Texture2DHandle : public TextureHandle
	{
		Texture2DHandle() = default;

		static Texture2DHandle	Null() { return Texture2DHandle{ 0 }; }

	};
}