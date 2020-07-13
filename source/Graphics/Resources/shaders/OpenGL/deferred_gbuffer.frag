#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout(binding = 0) uniform sampler2D diffuseMap;

layout(binding = 2) uniform sampler2D specularMap;

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

	// and the diffuse per-fragment color
	FragAlbedoSpec.rgb = texture(diffuseMap, vs_texCoords).rgb;
	// store specular intensity in FragAlbedoSpec's alpha component as an optimization.
	FragAlbedoSpec.a = texture(specularMap, vs_texCoords).r;
}