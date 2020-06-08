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
		VIEW_PROJECTION_PLANES,
		MATERIAL_VIEW_MAT3
	};

	enum  MaterialTextureBinding : uint8_t
	{
		DIFFUSE = 0,
		NORMAL,
		SPECULAR,
		EMISSION,
		GLOSS,
		SKYBOX,
		CUSTOM
	};

}