#version 330 core

in vec3 v_fragColor;

out vec4 FragColor;

void main()
{
	FragColor = vec4(v_fragColor, 1.0);
}