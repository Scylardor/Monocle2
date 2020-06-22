#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (triangles) in;

layout (triangle_strip, max_vertices=18) out;

out vec4 FragPos_world; // FragPos from GS (output per emitvertex)


layout (std140, binding = 9) uniform OmniShadowMappingInfo
{
	mat4	shadowViewProjMatrices[6];
	float	projectionFarPlane;
	vec3	lightPos_world;
};


void main()
{
	for (int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // built-in variable that specifies to which face we render.
		for(int i = 0; i < 3; ++i) // for each triangle vertex
		{
			FragPos_world = gl_in[i].gl_Position;

			// convert gl_Position to light space before projecting it
			gl_Position = shadowViewProjMatrices[face] * FragPos_world;
			EmitVertex();
		}

		EndPrimitive();
	}
}