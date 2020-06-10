#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vs_normal;
out vec4 vs_position_world;

flat out int error;

layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
	vec4	cameraPos_world;
};


layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};


void main()
{
	// TODO: we recompute the normal matrix in shader because ObjectMatrices normal matrix is actually built from the model view !!
	// fix that.
	vs_normal = mat3(transpose(inverse(model))) * normal;

	vs_position_world = model * vec4(position, 1.0);

	gl_Position = modelViewProjection * vec4(position, 1.0);
}
