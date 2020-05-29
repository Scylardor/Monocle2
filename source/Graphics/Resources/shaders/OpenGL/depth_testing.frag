#version 420 core

out vec4 FragColor;

in vec2 TexCoords;

layout (std140, binding = 6) uniform ProjectionPlanes
{
	float near;
	float far;
};

layout(binding = 0) uniform sampler2D diffuseMap;


float LinearizeDepth(float depth)
{
	// we have to first re-transform the depth values from the range [0,1] to normalized device coordinates in the range [-1,1].
	// Then we want to reverse the non-linear equation of the projection matrix and apply this inversed equation to the resulting depth value.
	// The result is then a linear depth value.
	float z = depth * 2.0 - 1.0; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
	FragColor = vec4(vec3(depth), 1.0);

	FragColor = texture(diffuseMap, TexCoords);
}