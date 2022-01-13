#pragma once
#include "Graphics/Color/Color.h"
#include "Math/Vec3.h"


namespace moe
{
	struct DirectionalLight
	{
		Vec3	Direction{ Vec3::ZeroVector() };

		Color3f	Ambient;
		Color3f	Diffuse;
		Color3f	Specular;
	};

	struct PointLight
	{
		Vec3	Position{ Vec3::ZeroVector() };

		Color3f	Ambient;
		Color3f	Diffuse;
		Color3f	Specular;

		float	ConstantAttenuation{ 0 };
		float	LinearAttenuation{ 0 };
		float	QuadraticAttenuation{ 0 };
	};

	struct SpotLight
	{
		Vec3	Position{ Vec3::ZeroVector() };
		Vec3	Direction{ Vec3::ZeroVector() };

		Color3f	Ambient;
		Color3f	Diffuse;
		Color3f	Specular;

		float	ConstantAttenuation{ 0 };
		float	LinearAttenuation{ 0 };
		float	QuadraticAttenuation{ 0 };

		float	InnerCutoff{ 0 };
		float	OuterCutoff{0};
	};





	struct LightObject
	{
		inline static const auto DIRECTIONAL_ATTENUATION = -1.f;
		inline static const auto OMNIDIRECTIONAL_CUTOFF = -1.f;

		Vec3	Position{0};
		float	ConstantAttenuation{ 0 };
		Vec3	Direction{ 0 };
		float	LinearAttenuation{ 0 };
		Color3f	Ambient{ 0 };
		float	QuadraticAttenuation{ 0 };
		Color3f	Diffuse{ 0 };
		float	SpotInnerCutoff{ 0 };
		Color3f	Specular{ 0 };
		float	SpotOuterCutoff{ 0 };
	};


}
