#pragma once

#include "Core/Misc/Types.h"

namespace moe
{
	enum  MaterialBlockBinding : uint16_t
	{
		FRAME_TIME = 0,
		FRAME_LIGHTS,
		VIEW_CAMERA,
		MATERIAL_PHONG,
		OBJECT_MATRICES,
		MATERIAL_COLOR,
	};

	enum  MaterialTextureBinding : uint8_t
	{
		DIFFUSE = 0,
		NORMAL,
		SPECULAR,
		GLOSS,
		CUSTOM
	};

}