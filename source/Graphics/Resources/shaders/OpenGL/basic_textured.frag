#version 420 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;


layout(binding=0) uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, TexCoord);
}