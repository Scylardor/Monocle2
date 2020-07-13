#version 420 core
// Require version 420 to be able to use "binding = ..." extension.


in vec2 vs_TexCoords;

out vec4 FragColor;


#define LIGHTS_NBR 32

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
};


layout (std140, binding = 10) uniform ToneMappingParameters
{
	bool	enabled;
	float	exposure;
	bool	useReinhard;
} ToneMapping;


layout(binding = 0) uniform sampler2D positionMap;

layout(binding = 1) uniform sampler2D normalsMap;

layout(binding = 2) uniform sampler2D albedoSpecularMap;


struct SurfaceInfo
{
	vec4 fragPosition;
	vec4 diffuseColor;
	vec3 normal;
	float specular;
};


vec4	ComputeDirectionalLight(int iLight, SurfaceInfo surfInfo)
{
	// First compute ambient because it will be used no matter what
	vec4 ambient  = lightsData[iLight].lightAmbient * surfInfo.diffuseColor;

	// Negate direction vector because we specify the light direction as pointing from the light source.
	// Therefore we negate the light direction to get a direction vector pointing towards the light source.
	vec4 lightDirEye = normalize(view * -lightsData[iLight].lightDirection);

	// Diffuse
	float diffuseStrength = max(dot(surfInfo.normal, lightDirEye.xyz), 0.0);
	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * surfInfo.diffuseColor;

	// Specular
	float specularStrength = 0.0;
	if (diffuse != 0) // Do not produce a specular highlight if the object is back lit.
	{
		vec3 vertToEyeDir = normalize(-surfInfo.fragPosition.xyz); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
		// Compute Blinn-Phong half vector
		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir);
		specularStrength = pow(max(dot(surfInfo.normal, halfwayDir), 0.0), 32.0); // NB : hardcoded shininess
	}

	vec4 specular = lightsData[iLight].lightSpecular * specularStrength * surfInfo.specular;

	return ambient + diffuse + specular;
}


vec4	ComputePointLight(int iLight, vec4 lightDirEye, float attenuation, SurfaceInfo surfInfo)
{
	// First compute ambient because it will be used no matter what
	vec4 ambient  = lightsData[iLight].lightAmbient * surfInfo.diffuseColor;

	// Diffuse
	float diffuseStrength = max(dot(surfInfo.normal, lightDirEye.xyz), 0.0);
	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * surfInfo.diffuseColor;

	// Specular
	float specularStrength = 0.0;
	if (diffuse != 0) // Do not produce a specular highlight if the object is back lit.
	{
		vec3 vertToEyeDir = normalize(-surfInfo.fragPosition.xyz); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
		// Compute Blinn-Phong half vector
		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir);
		specularStrength = pow(max(dot(surfInfo.normal, halfwayDir), 0.0), 32.0); // NB : hardcoded shininess
	}

	vec4 specular = lightsData[iLight].lightSpecular * specularStrength * surfInfo.specular;

	return ((ambient + diffuse + specular) * attenuation);
}


vec4	ComputeSpotLight(int iLight, vec4 lightDirEye, float attenuation, SurfaceInfo surfInfo)
{
	// First compute ambient because it will be used no matter what
	vec4 ambient  = lightsData[iLight].lightAmbient * surfInfo.diffuseColor;

	float theta = dot(lightDirEye, normalize(view * -lightsData[iLight].lightDirection)); // -lightDirection : same as above
	float epsilon = lightsData[iLight].lightSpotInnerCutoff - lightsData[iLight].lightSpotOuterCutoff;
	float intensity = clamp((theta - lightsData[iLight].lightSpotOuterCutoff) / epsilon, 0.0, 1.0);

	// Diffuse
	float diffuseStrength = max(dot(surfInfo.normal, lightDirEye.xyz), 0.0);
	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * surfInfo.diffuseColor;

	// Specular
	vec3 vertToEyeDir = normalize(-surfInfo.fragPosition.xyz); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
	// Compute Blinn-Phong half vector
	vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir);
	float specularStrength = pow(max(dot(surfInfo.normal, halfwayDir), 0.0), 32.0); // NB : hardcoded shininess
	vec4 specular = lightsData[iLight].lightSpecular * specularStrength * surfInfo.specular;

	return ((ambient + diffuse + specular) * attenuation * intensity);
}


void main()
{
	// retrieve data from G-buffer
	SurfaceInfo surfInfo;
	surfInfo.fragPosition = texture(positionMap, vs_TexCoords); // in eye space

	vec4 albedoSpec = texture(albedoSpecularMap, vs_TexCoords);
	surfInfo.diffuseColor = vec4(albedoSpec.rgb, 1.0);

	surfInfo.normal = normalize(texture(normalsMap, vs_TexCoords).rgb);

	surfInfo.specular = albedoSpec.a;

	// TODO: the "usual" lighting code has a unresolved bug (background takes color of nearest light), so I patched it with a code grab from LearnOpenGL. To fix !

	// then calculate lighting as usual
	vec4 lighting  = surfInfo.diffuseColor * 0.1; // hard-coded ambient component
	vec3 viewDir  = normalize(-surfInfo.fragPosition.xyz); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
	for(int i = 0; i < lightsNumber; ++i)
	{
		// diffuse
		vec3 lightDir = normalize(lightsData[i].lightPosition - surfInfo.fragPosition).xyz;
		vec4 diffuse = max(dot(surfInfo.normal, lightDir), 0.0) * surfInfo.diffuseColor * lightsData[i].lightDiffuse;
		// specular
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(surfInfo.normal, halfwayDir), 0.0), 16.0);
		vec4 specular = lightsData[i].lightSpecular * spec * surfInfo.specular;
		// attenuation
		float distance = length(lightsData[i].lightPosition - surfInfo.fragPosition);
		float attenuation = 1.0 / (lightsData[i].lightConstantAttenuation + lightsData[i].lightLinearAttenuation * distance + lightsData[i].lightQuadraticAttenuation * distance * distance);
		diffuse *= attenuation;
		specular *= attenuation;
		lighting += diffuse + specular;
	}
//	for (int iLight = 0; iLight < lightsNumber; iLight++)
//	{
//		if (lightsData[iLight].lightPosition.w == 0) // it's a directional light
//		{
//			FragColor += ComputeDirectionalLight(iLight, surfInfo);
//		}
//		else // it's a position light (point or spot) : start calculations
//		{
//			vec4 lightPosEye = (view * lightsData[iLight].lightPosition);
//			vec4 lightDirEye = lightPosEye - surfInfo.fragPosition;
//
//			float distance = length(lightDirEye);
//			float attenuation = 1.0 /
//			 (lightsData[iLight].lightConstantAttenuation + (lightsData[iLight].lightLinearAttenuation * distance) + (lightsData[iLight].lightQuadraticAttenuation * distance * distance));
//
//			lightDirEye = normalize(lightDirEye);
//
//			if (lightsData[iLight].lightDirection != vec4(0)) // it has position and direction : it's a spot light
//			{
//				FragColor += ComputeSpotLight(iLight, lightDirEye, attenuation, surfInfo);
//			}
//			else
//			{
//				FragColor += ComputePointLight(iLight, lightDirEye, attenuation, surfInfo);
//			}
//		}
//	}

	vec3 gammaCorrectedColor = lighting.rgb;

	if (ToneMapping.enabled)
	{
		if (ToneMapping.useReinhard)
		{
			// Reinhard tone mapping divides the entire HDR color values to LDR color values.
			// Evenly balances out all brightness values onto LDR, but it does tend to slightly favor brighter areas.
			gammaCorrectedColor /= (FragColor.rgb + vec3(1.0));
		}
		else // use exposure
		{
			gammaCorrectedColor = vec3(1.0) - exp(-gammaCorrectedColor * ToneMapping.exposure);
		}
	}

	// apply gamma correction
	const float gamma = 2.2;

	gammaCorrectedColor = pow(gammaCorrectedColor, vec3(1.0/gamma));
	FragColor = vec4(gammaCorrectedColor, 1.0);
}