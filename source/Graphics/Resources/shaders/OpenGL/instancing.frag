#version 420 core

in vec2 vs_TexCoords;
in vec4 color;

out vec4 FragColor;


layout(binding = 0) uniform sampler2D diffuseMap;

void main()
{

	FragColor = texture(diffuseMap, vs_TexCoords);
}