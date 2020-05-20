#version 420 core
// Require version 420 to be able to use "binding = ..." extension.


layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
};

layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};

out vec3 vs_normal;

out vec3 vs_fragPosEye;

void main()
{
	vs_normal = normalMatrix * normal;

	vec4 pos4 = vec4(position, 1.0);

	vs_fragPosEye = vec3(modelView * pos4);

	gl_Position = modelViewProjection * pos4;
}
