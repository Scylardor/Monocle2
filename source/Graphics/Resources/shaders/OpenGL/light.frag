
#version 330 core

layout (std140) uniform Color { vec4 colorValue; };

out vec4 FragColor;

void main()
{
    FragColor = colorValue;
}