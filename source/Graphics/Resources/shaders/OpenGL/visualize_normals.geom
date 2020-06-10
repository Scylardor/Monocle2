#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;


in VS_OUT {
	vec3 normal;
} gs_in[];


layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
	vec4	cameraPos_world;
};

const float MAGNITUDE = 0.4;

void GenerateLine(int index)
{
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();

	gl_Position = projection * (gl_in[index].gl_Position +
								vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
	EmitVertex();

	EndPrimitive();
}

void main()
{
	GenerateLine(0); // first vertex normal
	GenerateLine(1); // second vertex normal
	GenerateLine(2); // third vertex normal
}