#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

in vec3	vs_normal;
in vec2 vs_texCoords;
in vec3	vs_fragPosEye;


// For MRT (Multiple Render Target) rendering
layout (location = 0) out vec3 FragPosition;
layout (location = 1) out vec3 FragNormal;
layout (location = 2) out vec4 FragAlbedoSpec;


void main()
{
	// store the fragment position vector in the first gbuffer texture
	FragPosition = vs_fragPosEye;

	// also store the per-fragment normals into the gbuffer
	FragNormal = normalize(vs_normal);

	// and the diffuse per-fragment color. We ignore specular. Use constant for sake of SSAO
	FragAlbedoSpec.rgb = vec3(0.95); // texture(diffuseMap, vs_texCoords).rgb;
}