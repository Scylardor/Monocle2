#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"

namespace moe
{
	struct Vertex_Position
	{
		Vec3 Position{ 0.f };
	};

	struct Vertex_PosColor
	{
		Vec3 Position{ 0.f };
		Vec3 Color{ 1.f };
	};

	struct Vertex2D_PosUV
	{
		Vec2 Position{ 0.f };
		Vec2 Texture_UV0{ 0.f };
	};

	struct Vertex_PosUV
	{
		Vec3 Position{ 0.f };
		Vec2 Texture_UV0{ 0.f };
	};

	struct Vertex_PosColorUV
	{
		Vec3 Position{0.f};
		Vec3 Color{ 1.f };
		Vec2 Texture_UV0{0.f};
	};

	struct Vertex_PosNormalUV
	{
		Vec3 Position{ 0.f };
		Vec3 Normal{ 1.f };
		Vec2 Texture_UV0{ 0.f };
	};
}
