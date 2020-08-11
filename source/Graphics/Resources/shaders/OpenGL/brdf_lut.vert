#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;


out vec2 vs_TexCoords;

void main()
{
	vs_TexCoords = texCoords;

	gl_Position = vec4(position, 1.0);
}
