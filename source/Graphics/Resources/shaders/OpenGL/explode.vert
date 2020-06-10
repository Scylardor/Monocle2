#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out VS_OUT {
	vec2 texCoords;
} vs_out;



layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};


void main()
{
	vs_out.texCoords = texCoords;
	gl_Position = vec4(position, 1.0);
}
