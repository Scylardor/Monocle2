#version 430 core
// Require version 430 to be able to use "layout (std430, binding = 2) readonly buffer" extension.


struct Light
{
	vec4	Position;
	vec4	Direction;
	vec4	Ambient;
	vec4	Diffuse;
	vec4	Specular;
};

layout (std140, binding = 1) uniform ViewData
{
	vec4	positionWS;
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
}	View;



layout (std430, binding = 2) readonly buffer LightData
{
	Light array[];
}	Lights;


layout (binding = 0) uniform sampler2D T_Ambient;
layout (binding = 1) uniform sampler2D T_Diffuse;
layout (binding = 2) uniform sampler2D T_Specular;
layout (binding = 3) uniform sampler2D T_Emission;

float	SHININESS = 7.0;
float	DIRECTIONAL_ATTENUATION = -1.0;
float	OMNIDIRECTIONAL_CUTOFF = -1.0;


in VS_OUT {
	vec3	PosWS;
	vec3	NormalWS;
	vec2	UV0;
} fs_in;

out vec4	FragColor;


vec3 g_sampledAmbient	= texture2D(T_Ambient, fs_in.UV0).xyz;
vec3 g_sampledDiffuse	= texture2D(T_Diffuse, fs_in.UV0).xyz;
vec3 g_sampledSpecular	= texture2D(T_Specular, fs_in.UV0).xyz;
vec3 g_sampledEmission	= texture2D(T_Emission, fs_in.UV0).xyz;

vec3 PhongDirectionalLight(vec3 PosWS, vec3 normalWS, vec3 viewDirWS, int iLight)
{
	float constantAttenuation = Lights.array[iLight].Position.w;
	vec3 ambient = Lights.array[iLight].Ambient.xyz * g_sampledAmbient * constantAttenuation;

	// Direction towards the light position
	vec3 L = -Lights.array[iLight].Direction.xyz; // assume it was sent normalized...

	float LDotN = max(dot(normalWS, L), 0.0);

	vec3 diffuse = Lights.array[iLight].Diffuse.xyz * g_sampledDiffuse * LDotN;

	vec3 specular = vec3(0.0);
	if (LDotN > 0.0)
	{
		// Reflection of the light ray on the surface
		vec3 R = reflect(-L, normalWS);
		float RDotV = max(dot(viewDirWS, R), 0.0);

		specular = Lights.array[iLight].Specular.xyz * g_sampledSpecular * pow(RDotV, SHININESS);
	}

	return ambient + diffuse + specular;
}


vec3 PhongPointLight(vec3 PosWS, vec3 normalWS, vec3 viewDirWS, int iLight)
{
	float constantAttenuation = Lights.array[iLight].Position.w;

	vec3 ambient = Lights.array[iLight].Ambient.xyz * g_sampledAmbient * constantAttenuation;

	// Direction towards the light position
	vec3 lightVec = Lights.array[iLight].Position.xyz - PosWS;
	vec3 L = normalize(lightVec);

	float LDotN = max(dot(normalWS, L), 0.0);

	vec3 diffuse = Lights.array[iLight].Diffuse.xyz * g_sampledDiffuse * LDotN;

	vec3 specular = vec3(0.0);
	if (LDotN > 0.0)
	{
		// Reflection of the light ray on the surface
		vec3 R = reflect(-L, normalWS);
		float RDotV = max(dot(viewDirWS, R), 0.0);

		specular = Lights.array[iLight].Specular.xyz * g_sampledSpecular * pow(RDotV, SHININESS);

		float linearAttenuation = Lights.array[iLight].Direction.w;
		float quadraticAttenuation = Lights.array[iLight].Ambient.w;
		float distance = length(lightVec);
		float attenuation = 1.0 / (constantAttenuation + linearAttenuation * distance + quadraticAttenuation * (distance * distance));
		diffuse *= attenuation;
		specular *= attenuation;
	}

	return ambient + diffuse + specular;
}


vec3 PhongSpotLight(vec3 PosWS, vec3 normalWS, vec3 viewDirWS, int iLight)
{
	float constantAttenuation = Lights.array[iLight].Position.w;

	// First compute ambient because it will be used no matter what
	vec3 ambient = Lights.array[iLight].Ambient.xyz * g_sampledAmbient * constantAttenuation;

	// Direction towards the light position
	vec3 lightVec = Lights.array[iLight].Position.xyz - PosWS;
	vec3 L = normalize(lightVec);

	float theta = dot(L, normalize(-Lights.array[iLight].Direction.xyz));

	float spotInnerCutoff = Lights.array[iLight].Diffuse.w;
	float spotOuterCutoff = Lights.array[iLight].Specular.w;
	float epsilon = spotInnerCutoff - spotOuterCutoff;
	float intensity = clamp((theta - spotOuterCutoff) / epsilon, 0.0, 1.0);

	// Diffuse
	float LDotN = max(dot(normalWS, L), 0.0);

	vec3 diffuse = Lights.array[iLight].Diffuse.xyz * g_sampledDiffuse * LDotN * intensity;

	// Specular
	vec3 specular = vec3(0.0);
	if (LDotN > 0.0)
	{
		// Reflection of the light ray on the surface
		vec3 R = reflect(-L, normalWS);
		float RDotV = max(dot(viewDirWS, R), 0.0);

		specular = Lights.array[iLight].Specular.xyz * g_sampledSpecular * pow(RDotV, SHININESS) * intensity;

		float linearAttenuation = Lights.array[iLight].Direction.w;
		float quadraticAttenuation = Lights.array[iLight].Ambient.w;
		float distance = length(lightVec);
		float attenuation = 1.0 / (constantAttenuation + linearAttenuation * distance + quadraticAttenuation * (distance * distance));
		diffuse *= attenuation;
		specular *= attenuation;
	}

	return ambient + diffuse + specular;
}

vec3 PhongLightingModel(vec3 PosWS, vec3 normalWS, int nbLights)
{
	// Direction towards the viewer position
	vec3 viewDirWS = normalize(View.positionWS.xyz - PosWS);

	vec3 lightingColor = vec3(0);
	for (int i = 0; i < nbLights; ++i)
	{
		if (Lights.array[i].Position.w == DIRECTIONAL_ATTENUATION) // this is a directional light
			lightingColor += PhongDirectionalLight(PosWS, normalWS, viewDirWS, i);
		else if (Lights.array[i].Diffuse.w == OMNIDIRECTIONAL_CUTOFF) // this is a point light
			lightingColor += PhongPointLight(PosWS, normalWS, viewDirWS, i);
		else // this is a spot light
			lightingColor += PhongSpotLight(PosWS, normalWS, viewDirWS, i);
	}

	return lightingColor;
}


void main()
{
	vec3 fragNormalWS = normalize(fs_in.NormalWS);
	vec3 color = vec3(1, 0, 1);
	int nbLights = Lights.array.length();
	color = PhongLightingModel(fs_in.PosWS, fragNormalWS, nbLights);

	FragColor = vec4(color, 1);
}