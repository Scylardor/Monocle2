#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv0;

layout (std140, binding = 0) uniform ObjectMatrices
{
	mat4	MVP;
};

out VS_OUT {
	vec2 UV0;
} vs_out;


out vec2 TexCoord;

void main()
{
	vs_out.UV0 = uv0;

	gl_Position = MVP * vec4(position, 1.0);
}
