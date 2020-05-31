#version 420 core

out vec4 FragColor;

in vec2 TexCoords;


layout(binding = 0) uniform sampler2D diffuseMap;


void main()
{
    vec4 texColor = texture(diffuseMap, TexCoords);
    if(texColor.a < 0.1)
        discard;

    FragColor = texColor;
}