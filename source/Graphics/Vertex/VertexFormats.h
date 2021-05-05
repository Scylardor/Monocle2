#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace moe
{
	struct BasicVertex
	{
		glm::vec3 Position{0.f};
		glm::vec3 Color{ 1.f };
		glm::vec2 Texture_UV0{0.f};
	};
}
