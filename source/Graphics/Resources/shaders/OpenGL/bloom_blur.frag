#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

out vec4 FragColor;

in vec2 vs_texCoords;

#define TWO_PASS_GAUSSIAN_BLUR_MAX_WEIGHTS 32

layout (std140, binding = 11) uniform TwoPassGaussianBlurParams
{
	bool			horizontal; // is this the horizontal pass or the vertical pass ? Use int instead of bool for std140 requirements
	// Dimensions of the texture to sample
	int				textureWidth;
	int				textureHeight;
	uint			weightsSize;	// number of weights to read in the buffer
	float			weightsBuffer[TWO_PASS_GAUSSIAN_BLUR_MAX_WEIGHTS];
}	GaussBlurParams;


layout(binding = 0) uniform sampler2D image;



void main()
{
	vec2 tex_offset = 1.0 / ivec2(GaussBlurParams.textureWidth, GaussBlurParams.textureHeight); // gets size of single texel
	vec3 result = texture(image, vs_texCoords).rgb * GaussBlurParams.weightsBuffer[0]; // current fragment's contribution

	if (GaussBlurParams.horizontal)
	{
		for (int iWeight = 1; iWeight < GaussBlurParams.weightsSize; ++iWeight)
		{
			result += texture(image, vs_texCoords + vec2(tex_offset.x * iWeight, 0.0)).rgb * GaussBlurParams.weightsBuffer[iWeight];
			result += texture(image, vs_texCoords - vec2(tex_offset.x * iWeight, 0.0)).rgb * GaussBlurParams.weightsBuffer[iWeight];
		}
	}
	else
	{
		for (int iWeight = 1; iWeight < GaussBlurParams.weightsSize; ++iWeight)
		{
			result += texture(image, vs_texCoords + vec2(0.0, tex_offset.y * iWeight)).rgb * GaussBlurParams.weightsBuffer[iWeight];
			result += texture(image, vs_texCoords - vec2(0.0, tex_offset.y * iWeight)).rgb * GaussBlurParams.weightsBuffer[iWeight];
		}
	}

	FragColor = vec4(result, 1.0);
}