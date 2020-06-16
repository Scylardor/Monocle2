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
		MATERIAL_SKYBOX_VIEWPROJ
	};

	enum  MaterialTextureBinding : uint8_t
	{
		DIFFUSE = 0,
		NORMAL,
		SPECULAR,
		EMISSION,
		GLOSS,
		SKYBOX
	};

	enum MaterialSamplerBinding : uint8_t
	{
		SAMPLER_0 = 0,
		SAMPLER_1,
		SAMPLER_2,
		SAMPLER_3,
		SAMPLER_4
	};

}