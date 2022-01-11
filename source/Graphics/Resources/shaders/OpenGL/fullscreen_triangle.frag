#version 420 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D u_Albedo;

void main()
{
	vec3 color = texture(u_Albedo, TexCoords).rgb;
	FragColor = vec4(color, 1.0);
}