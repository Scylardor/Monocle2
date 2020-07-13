#version 420 core

in vec2 TexCoords;

out float FragColor;


layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
}	Camera;


#define MAX_SSAO_KERNEL_SIZE 64

layout (std140, binding = 12) uniform SSAOParams
{
	float	radius;
	float	bias;
	vec2	noiseScale;
	int		kernelSize;
	vec4	kernel[MAX_SSAO_KERNEL_SIZE]; // Sending vec4's for std140 alignment reasons but really we only use the vec3 part
}	AmbientOcclusion;


layout(binding = 0) uniform sampler2D positionMap;

layout(binding = 1) uniform sampler2D normalsMap;

layout(binding = 2) uniform sampler2D ssaoNoiseTexture;


void main()
{
	// get inputs for SSAO algorithm
	vec3 fragPos = texture(positionMap, TexCoords).xyz;
	vec3 normal = normalize(texture(normalsMap, TexCoords).rgb);
	vec3 randomVec = normalize(texture(ssaoNoiseTexture, TexCoords * AmbientOcclusion.noiseScale).xyz);

	// create TBN change-of-basis matrix: from tangent-space to view-space
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;

	for (int iSample = 0; iSample < AmbientOcclusion.kernelSize; ++iSample)
	{
		// get sample position
		vec3 occSample = TBN * AmbientOcclusion.kernel[iSample].xyz; // from tangent to view-space
		occSample = fragPos + occSample * AmbientOcclusion.radius;

		// We then need to transform the sample to screen-space so we can sample its position/depth value as if we were rendering its position directly to the screen.
		// It's currently in view-space
		vec4 offset = vec4(occSample, 1.0);
		offset = Camera.projection * offset;    // from view to clip-space
		offset.xyz /= offset.w;               // perspective divide
		offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

		// get sample depth
		float sampleDepth = texture(positionMap, offset.xy).z; // get depth value of kernel sample

		// We introduce a range check that makes sure a fragment contributes to the occlusion factor if its depth values is within the sample's radius.
		// Otherwise, a fragment close to the edge of a surface will also consider depth values of surfaces far behind, incorrectly contributing to the occlusion factor.
		// We can then accumulate more occlusion based on whether the depth of the fragment is higher than the depth stored in the position map at this position.
		float rangeCheck = smoothstep(0.0, 1.0, AmbientOcclusion.radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= occSample.z + AmbientOcclusion.bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / AmbientOcclusion.kernelSize);

	FragColor = occlusion;

}