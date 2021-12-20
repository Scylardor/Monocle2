#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uv0;

layout (std140, binding = 0) uniform ObjectMatrices
{
	mat4 model;
};

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	ourColor = color;
    TexCoord = uv0;
	
	gl_Position = model * vec4(position, 1.0);
}
