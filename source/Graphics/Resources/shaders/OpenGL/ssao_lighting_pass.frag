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

layout(binding = 9) uniform sampler2D ambientOcclusionMap;

struct SurfaceInfo
{
	vec4 fragPosition;
	vec4 diffuseColor;
	vec3 normal;
	float specular;
	float ambientOcclusion;
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

	surfInfo.normal = normalize(texture(normalsMap, vs_TexCoords).rgb);

	vec4 albedoSpec = texture(albedoSpecularMap, vs_TexCoords);
	surfInfo.diffuseColor = vec4(albedoSpec.rgb, 1.0);

	surfInfo.specular = albedoSpec.a;

	surfInfo.ambientOcclusion = texture(ambientOcclusionMap, vs_TexCoords).r;

	vec3 ambient = vec3(0.3 * surfInfo.diffuseColor.rgb * surfInfo.ambientOcclusion); // TOFIX: hard-coded ambient component!
	vec3 lighting  = ambient;

	// TODO: the "usual" lighting code has a unresolved bug (background takes color of nearest light), so I patched it with a code grab from LearnOpenGL. To fix !
	for(int iLight = 0; iLight < lightsNumber; ++iLight)
	{
		// then calculate lighting as usual
		vec3 viewDir  = normalize(-surfInfo.fragPosition.xyz); // viewpos is (0.0.0)
		vec3 lightPosView = vec3(view * lightsData[iLight].lightPosition);
		// diffuse
		vec3 lightDir = normalize(lightPosView - surfInfo.fragPosition.xyz);
		vec3 diffuse = max(dot(surfInfo.normal, lightDir), 0.0) * surfInfo.diffuseColor.rgb * lightsData[iLight].lightDiffuse.rgb;
		// specular
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(surfInfo.normal, halfwayDir), 0.0), 8.0); // TOFIX: hard-coded specular exponent!
		vec3 specular = lightsData[iLight].lightSpecular.rgb * spec;
		// attenuation
		float distance = length(lightPosView - surfInfo.fragPosition.xyz);
		float attenuation = 1.0 / (lightsData[iLight].lightConstantAttenuation + lightsData[iLight].lightLinearAttenuation * distance + lightsData[iLight].lightQuadraticAttenuation * distance * distance);

		diffuse *= attenuation;
		specular *= attenuation;
		lighting += diffuse + specular;
	}

	FragColor = vec4(lighting, 1.0);


//	vec3 gammaCorrectedColor = lighting.rgb;
//
//	if (ToneMapping.enabled)
//	{
//		if (ToneMapping.useReinhard)
//		{
//			// Reinhard tone mapping divides the entire HDR color values to LDR color values.
//			// Evenly balances out all brightness values onto LDR, but it does tend to slightly favor brighter areas.
//			gammaCorrectedColor /= (FragColor.rgb + vec3(1.0));
//		}
//		else // use exposure
//		{
//			gammaCorrectedColor = vec3(1.0) - exp(-gammaCorrectedColor * ToneMapping.exposure);
//		}
//	}
//
//	// apply gamma correction
//	const float gamma = 2.2;
//
//	gammaCorrectedColor = pow(gammaCorrectedColor, vec3(1.0/gamma));
//	FragColor = vec4(lighting, 1.0);
}