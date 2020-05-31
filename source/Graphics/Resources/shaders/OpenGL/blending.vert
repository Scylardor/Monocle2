#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;


layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};


void main()
{
	TexCoords = aTexCoords;
	gl_Position = modelViewProjection * vec4(aPos, 1.0);
}