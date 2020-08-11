#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

in vec3 vs_fragPosWorld;

out vec4 FragColor;

layout(binding = 0) uniform samplerCube environmentMap;

const float PI = 3.14159265359;

const vec3 upVector = vec3(0.0, 1.0, 0.0);

void main()
{
	// The world vector acts as the normal of a tangent surface
	// from the origin, aligned to WorldPos. Given this normal, calculate all
	// incoming radiance of the environment. The result of this radiance
	// is the radiance of light coming from -Normal direction, which is what
	// we use in the PBR shader to sample irradiance.
	vec3 N = normalize(vs_fragPosWorld);

	vec3 irradiance = vec3(0.0);

	// tangent space calculation from origin point
	// Use Gram-Schmidt to recompose a tangent-space coordinate space with the normal as forward vector.
	vec3 right = cross(upVector, N);
	vec3 up = cross(N, right);

	// TODO : the smaller the delta, the more precise the convolution is. Make that parameterable.
	const float sampleDelta = 0.025;
	int nbrSamples = 0;

	// Use the Riemann sum to approximate the double integral needed along a hemisphere oriented around the sample direction.
	// Note that we scale the sampled color value by cos(theta) due to the light being weaker at larger angles,
	// and by sin(theta) to account for the smaller sample areas in the higher hemisphere areas.
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nbrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nbrSamples));

	FragColor = vec4(irradiance, 1.0);
}
