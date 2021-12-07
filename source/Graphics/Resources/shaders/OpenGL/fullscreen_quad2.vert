#version 420 core

layout (location = 0) in vec2 v_Pos;
layout (location = 1) in vec2 v_UV0;

out vec2 UV0;

void main()
{
    UV0 = v_UV0;

    gl_Position = vec4(v_Pos.x, v_Pos.y, 0.0, 1.0);
}