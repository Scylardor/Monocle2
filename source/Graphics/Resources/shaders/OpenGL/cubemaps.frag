#version 420 core

in vec3 vs_normal;

in vec4 vs_position_world;

flat in int error;

out vec4 FragColor;


layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
	vec4	cameraPos_world;
};

layout(binding = 5) uniform samplerCube skybox;


void main()
{
	// Reflection - first compute the light-to-point world space vector...
	vec3 eyeToPoint = normalize(vs_position_world.xyz - cameraPos_world.xyz);
	// ... Then reflect it around the surface normal to obtain the vector we lookup the skybox with
	vec3 reflectVec = reflect(eyeToPoint, normalize(vs_normal));
	FragColor = vec4(texture(skybox, reflectVec).rgb, 1.0);

}