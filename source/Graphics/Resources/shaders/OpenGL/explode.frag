#version 420 core

in vec2 v_texCoords;

out vec4 FragColor;


layout(binding = 0) uniform sampler2D diffuseMap;

void main()
{
	FragColor = texture(diffuseMap, v_texCoords);
}