#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexUV0;

layout (std140, binding = 0) uniform ObjectMatrices
{
	mat4	MVP;
	mat4	ModelView;
	mat4	Normal;
	mat4	Model;
}	Matrices;

out VS_OUT {
	vec3	PosVS;
	vec3	NormalVS;
	vec2	UV0;
} vs_out;

void main()
{
	vec4 position = vec4(VertexPosition, 1.0);

	vs_out.NormalVS = normalize(mat3(Matrices.Normal) * VertexNormal);
	vs_out.PosVS = (Matrices.ModelView * position).xyz;
	vs_out.UV0 = VertexUV0;

	gl_Position = Matrices.MVP * position;
}
