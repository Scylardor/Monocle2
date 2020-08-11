#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;

layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
	vec4	cameraPos;
}	Camera;

out vec3 vs_fragPos;

void main()
{
	vs_fragPos = position;

	mat4 rotView = mat4(mat3(Camera.view)); // remove translation from the view matrix
	vec4 clipPos = Camera.projection * rotView * vec4(vs_fragPos, 1.0);

	// this xyww trick ensures the depth value of the rendered cube fragments always end up at 1.0, the maximum depth value
	gl_Position = clipPos.xyww;
}