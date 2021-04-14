#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform PushConstants
{
    mat4 MVP;
} ObjectBlock;


void main()
{
    gl_Position = ObjectBlock.MVP * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}