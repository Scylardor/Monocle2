#version 420 core
// Require version 420 to be able to use "binding = ..." extension.


in vec3 vs_fragPos;


out vec4 FragColor;


layout(binding = 0) uniform samplerCube environmentMap;


void main()
{
	vec3 envColor = texture(environmentMap, vs_fragPos).rgb;

	// HDR tonemap and gamma correct
	// TODO: hook this to the uniform gamma / tone mapping structures.
	envColor = envColor / (envColor + vec3(1.0));

	envColor = pow(envColor, vec3(1.0/2.2));

	FragColor = vec4(envColor, 1.0);
}
