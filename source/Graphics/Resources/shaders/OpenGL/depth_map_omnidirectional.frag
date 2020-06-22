#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

in vec4 FragPos_world;

layout (std140, binding = 9) uniform OmniShadowMappingInfo
{
	mat4	shadowViewProjMatrices[6];
	float	projectionFarPlane;
	vec3	lightPos_world;
};

void main()
{
	// get distance between fragment and light source
	float lightDistance = length(FragPos_world.xyz - lightPos_world);

	// map to [0;1] range by dividing by far_plane
	lightDistance = lightDistance / projectionFarPlane;

	// write this as modified depth
	gl_FragDepth = lightDistance;
}