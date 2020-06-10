#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VS_OUT {
	vec3 normal;
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
	vs_out.normal = normalize(vec3(vec4(normalMatrix * normal, 0.0)));

	gl_Position = modelView * vec4(position, 1.0);
}
