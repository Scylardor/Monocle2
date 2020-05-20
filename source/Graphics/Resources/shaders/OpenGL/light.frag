#version 420 core
// Require version 420 to be able to use "binding = ..." extension.


layout (std140, binding = 5) uniform Color
{
	vec4 colorValue;
};

out vec4 FragColor;

void main()
{
	FragColor = colorValue;
}