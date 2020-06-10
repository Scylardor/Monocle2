#version 420 core

out vec4 FragColor;

in vec3 TexCoords;

layout(binding = 5) uniform samplerCube skybox;


void main()
{
	FragColor = texture(skybox, TexCoords);
}