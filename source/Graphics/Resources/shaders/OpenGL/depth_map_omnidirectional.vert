#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;


layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};


void main()
{
	// in this shader, just send the world-space coordinates to the geometry shader.
	gl_Position = model * vec4(position, 1.0);
}
