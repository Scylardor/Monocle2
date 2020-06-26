#version 420 core

out vec4 FragColor;

in vec2 TexCoords;

layout (std140, binding = 10) uniform ToneMappingParameters
{
	bool	enabled;
	float	exposure;
	bool	useReinhard;
} ToneMapping;


layout(binding = 0) uniform sampler2D diffuseMap;


void main()
{
	// apply gamma correction
	const float gamma = 2.2;

	vec4 colorValue = texture(diffuseMap, TexCoords);

	vec3 result = colorValue.rgb;

	if (ToneMapping.enabled)
	{
		if (ToneMapping.useReinhard)
		{
			// Reinhard tone mapping divides the entire HDR color values to LDR color values.
			// Evenly balances out all brightness values onto LDR, but it does tend to slightly favor brighter areas.
			result /= (result + vec3(1.0));
		}
		else // use exposure
		{
			result = vec3(1.0) - exp(-result * ToneMapping.exposure);
		}
	}

	FragColor.rgb = pow(result, vec3(1.0/gamma));
	FragColor.w = 1.0;
}