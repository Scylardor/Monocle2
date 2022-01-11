#version 420 core

in vec2 TexCoord;

out vec4 FragColor;

layout(binding=0) uniform sampler2D ourTexture;
layout(binding=1) uniform sampler2D ourTexture2;

void main()
{
	vec4 color1 = texture(ourTexture, TexCoord);
	vec4 color2 = texture(ourTexture2, TexCoord);
	
	// texture1 does not have alpha in the basic example (it's RGB), so 'mix' function dropped alpha from texture2.
	// Manually alpha blend here so that transparent texels of texture2 don't look white.
	FragColor = mix(color1.rgba, color2.rgba, 0.2 * color2.a);
}