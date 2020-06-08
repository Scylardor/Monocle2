#version 420 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D diffuseMap;

//void main()
//{
//
//	// Negative colors:
//	//FragColor = vec4(vec3(1.0 - texture(diffuseMap, TexCoords)), 1.0);
//
//	// Unweighted grayscale effect:
//	//	FragColor = texture(diffuseMap, TexCoords);
//	//	float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//	//	FragColor = vec4(average, average, average, 1.0);
//
//	// Weighted grayscale effect:
//	FragColor = texture(diffuseMap, TexCoords);
//	float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//	FragColor = vec4(average, average, average, 1.0);
//}



// Convolution Kernels


const float offset = 1.0 / 300.0;

void main()
{
	vec2 offsets[9] = vec2[](
		vec2(-offset,  offset), // top-left
		vec2( 0.0f,    offset), // top-center
		vec2( offset,  offset), // top-right
		vec2(-offset,  0.0f),   // center-left
		vec2( 0.0f,    0.0f),   // center-center
		vec2( offset,  0.0f),   // center-right
		vec2(-offset, -offset), // bottom-left
		vec2( 0.0f,   -offset), // bottom-center
		vec2( offset, -offset)  // bottom-right
	);


	// Sharpen kernel
	float sharpenKernel[9] = float[](
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);

	// Blur kernel
	const float invSixteen = 1.0 / 16.0;
	float blurKernel[9] = float[](
		1.0 * invSixteen, 2.0 * invSixteen, 1.0 * invSixteen,
		2.0 * invSixteen, 4.0 * invSixteen, 2.0 * invSixteen,
		1.0 * invSixteen, 2.0 * invSixteen, 1.0 * invSixteen
	);

	// Edge detection kernel
	float edgeDetectKernel[9] = float[](
		1, 1,  1,
		1, -8, 1,
		1, 1,  1
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(diffuseMap, TexCoords.st + offsets[i]));
	}

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] * edgeDetectKernel[i];


	FragColor = vec4(col, 1.0);
}