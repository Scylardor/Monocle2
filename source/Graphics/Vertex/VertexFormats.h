#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace moe
{
	struct Vertex_PosColor
	{
		glm::vec3 Position{ 0.f };
		glm::vec3 Color{ 1.f };
	};

	struct Vertex2D_PosUV
	{
		glm::vec2 Position{ 0.f };
		glm::vec2 Texture_UV0{ 0.f };
	};

	struct Vertex_PosColorUV
	{
		glm::vec3 Position{0.f};
		glm::vec3 Color{ 1.f };
		glm::vec2 Texture_UV0{0.f};
	};
}
