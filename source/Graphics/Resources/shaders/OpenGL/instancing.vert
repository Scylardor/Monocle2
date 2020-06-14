#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in mat4 instanceModelMatrix;

out vec2 vs_TexCoords;
out vec4 color;

layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
	vec4	cameraPos_world;
};


void main()
{
	mat4 testMatrix;
	testMatrix[0] = vec4(1, 0, 0, 0);
	testMatrix[1] = vec4(0, 1, 0, 0);
	testMatrix[2] = vec4(0, 0, 1, 0);
	testMatrix[3] = vec4(0, 0, 0, 1);

	mat4 modelMatrix = mat4(1.0);

	modelMatrix[3][0] = gl_InstanceID * 2.0;
	gl_Position = viewProjection * instanceModelMatrix * vec4(position, 1.0);
	vs_TexCoords = texCoords;
}