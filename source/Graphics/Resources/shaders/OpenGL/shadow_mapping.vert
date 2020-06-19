#version 420 core
// Require version 420 to be able to use "binding = ..." extension.


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
	vec3 FragEyePos;
	vec4 FragLightSpacePos;
	vec3 FragEyeNormal;
	vec2 FragTexCoords;
} vs_out;


layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};


layout (std140, binding = 8) uniform ShadowMappingInfo
{
	mat4 lightSpaceMatrix;
};



out vec3 vs_normal;
out vec2 vs_texCoords;

out vec3 vs_fragPosEye;

void main()
{
	vs_out.FragEyeNormal = normalMatrix * normal;
	vs_out.FragTexCoords = texCoords;

	vec4 pos4 = vec4(position, 1.0);

	vs_out.FragEyePos = vec3(modelView * pos4);

	vs_out.FragLightSpacePos = lightSpaceMatrix * model * pos4;

	gl_Position = modelViewProjection * pos4;
}
