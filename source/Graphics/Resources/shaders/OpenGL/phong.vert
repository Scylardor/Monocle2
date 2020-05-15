#version 330 core

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (std140) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};

out vec3 vs_normal;

out vec3 vs_fragPosWorld;

void main()
{
	vs_normal = normalMatrix * normal;

	vec4 pos4 = vec4(position, 1.0);

	vs_fragPosWorld = vec3(model * pos4);

	gl_Position = modelViewProjection * pos4;
}
