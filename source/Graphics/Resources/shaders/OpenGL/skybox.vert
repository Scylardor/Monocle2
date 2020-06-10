#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;


layout (std140, binding = 7) uniform SkyboxViewProjection
{
	mat4 skyboxViewProj;
};


void main()
{
	TexCoords = aPos;
	vec4 pos = skyboxViewProj * vec4(aPos, 1.0);

	// For a skybox, we need to trick the depth buffer into believing that the skybox has the maximum depth value of 1.0.
	// This is to make it fail the depth test wherever there's a different object in front of it : we want to render the skybox behind everything else.
	// we can set the z component of the output position equal to its w component which will result in a z component that is always equal to 1.0,
	// because the perspective division (divide by w) will thus set it to (w/w) = 1.0 : i.e. the maximum depth value.
	pos.z = pos.w;
	gl_Position = pos;
}