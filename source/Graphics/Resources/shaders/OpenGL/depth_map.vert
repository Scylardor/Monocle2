#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;


layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};


void main()
{
	// in this shader, modelViewProjection has actually been built with a lightSpaceMatrix.
	// The light is acting like a camera watching in the direction of the light rays, using a custom projection.
	gl_Position = modelViewProjection * vec4(position, 1.0);
}
