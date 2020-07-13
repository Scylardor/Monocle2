#version 420 core

out float FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D occlusionMap;

void main()
{
	const vec2 texelSize = 1.0 / vec2(textureSize(occlusionMap, 0));

	const float blurKernelExtent = 4.0;
	const int halfExtent = int(blurKernelExtent * 0.5);

	float result = 0.0;

	for (int x = -halfExtent; x < halfExtent; ++x)
	{
		for (int y = -halfExtent; y < halfExtent; ++y)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(occlusionMap, TexCoords + offset).r;
		}
	}

	const float kernelSamplesNbr = blurKernelExtent * blurKernelExtent;

	FragColor = result / kernelSamplesNbr;
}