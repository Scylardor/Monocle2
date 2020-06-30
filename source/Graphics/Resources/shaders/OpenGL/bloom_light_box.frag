#version 420 core
// Require version 420 to be able to use "binding = ..." extension.


layout (std140, binding = 5) uniform Color
{
	vec4 colorValue;
}	LightColor;


layout (location = 0) out vec4 FragColor;

layout (location = 1) out vec4 BrightColor; // For Bloom pass MRT (Multiple Render Target)



void main()
{
	FragColor = LightColor.colorValue;

	// check whether fragment output is higher than threshold, if so output as brightness color
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

	if (brightness > 1.0)
		BrightColor = FragColor;
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}