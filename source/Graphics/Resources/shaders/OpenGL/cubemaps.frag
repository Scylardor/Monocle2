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
	// first compute the light-to-point world space vector...
	vec3 eyeToPoint = normalize(vs_position_world.xyz - cameraPos_world.xyz);

	// Reflection: reflect this vector around the surface normal to obtain the vector we lookup the skybox with
	// This creates a "perfect mirror" (no reflection map).
	//	vec3 reflectVec = reflect(eyeToPoint, normalize(vs_normal));
	//	FragColor = vec4(texture(skybox, reflectVec).rgb, 1.0);

	// Refraction: the direction of the view vector is slightly bent. This resulting bent vector is then used to sample from the cubemap.
	float ratio = 1.00 / 1.52; // The ratio between source material / destination material refracting indices. 1 = air and 1.52 = glass
	vec3 refractVec = refract(eyeToPoint, normalize(vs_normal), ratio);
	FragColor = vec4(texture(skybox, refractVec).rgb, 1.0);
}