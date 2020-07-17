#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};

out vec3 VertexNormal;
out vec2 vs_texCoords;

out vec3 vs_fragPosWorld;

void main()
{
	// TODO: to fix...
	VertexNormal =  transpose(inverse(mat3(model))) * normal;
	vs_texCoords = texCoords;

	vec4 pos4 = vec4(position, 1.0);

	vs_fragPosWorld = vec3(model * pos4);

	gl_Position = modelViewProjection * pos4;
}
