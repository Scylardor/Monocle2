#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;


out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos; // so far, manage only a single light...
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;



#define LIGHTS_NBR 16

struct LightData
{
	vec4	lightPosition;
	vec4	lightDirection;
	vec4	lightAmbient;
	vec4	lightDiffuse;
	vec4	lightSpecular;
	float	lightConstantAttenuation;
	float	lightLinearAttenuation;
	float	lightQuadraticAttenuation;
	float	lightSpotInnerCutoff;
	float	lightSpotOuterCutoff;
};


layout (std140, binding = 1) uniform LightCastersData
{
	uint		lightsNumber;
	LightData	lightsData[LIGHTS_NBR];
};


layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
	vec4	cameraPos_world;
};

layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};



out vec3 vs_normal;
out vec2 vs_texCoords;

out vec3 vs_fragPosEye;

void main()
{
	// TODO: to fix !!
	// Recomputing the normal matrix in shader because right now uniform one outputs a view space transform!
	mat3 normalMat = mat3(transpose(inverse(model)));

	// Constructing the TBN matrix for normal mapping
	vec3 T = normalize(normalMat * tangent);
	vec3 B = normalize(normalMat * bitangent);
	vec3 N = normalize(normalMat * normal);

	// Store the inverse of the TBN matrix because we want to make our lighting calculations in tangent space
	// So we want the matrix to go from world space to tangent space, not the other way around!
	// Fortunately, since it's an orthonormal matrix we can just transpose it.
	mat3 TBN = transpose(mat3(T, B, N));

	vec4 pos4 = vec4(position, 1.0);

	vs_out.FragPos = vec3(model * pos4); // is this still useful?
	vs_out.TexCoords = texCoords;
	vs_out.TangentLightPos = TBN * lightsData[0].lightPosition.xyz;
	vs_out.TangentViewPos  = TBN * cameraPos_world.xyz;
	vs_out.TangentFragPos  = TBN * vs_out.FragPos;

	gl_Position = modelViewProjection * pos4;
}
