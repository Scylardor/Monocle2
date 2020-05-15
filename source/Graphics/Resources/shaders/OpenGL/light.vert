#version 330 core

layout(location = 0) in vec3 position;

layout (std140) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};


void main()
{
	gl_Position = modelViewProjection * vec4(position, 1.0);
}