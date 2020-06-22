#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

#define LIGHTS_NBR 16

in VS_OUT {
	vec3 FragEyePos;
	vec3 FragWorldPos;
	vec3 FragEyeNormal;
	vec2 FragTexCoords;
} fs_in;


out vec4	FragColor;


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

layout (std140, binding = 3) uniform PhongMaterial
{
	vec4	materialAmbient;
	vec4	materialDiffuse;
	vec4	materialSpecular;
	float	shininess;
};


layout (std140, binding = 9) uniform OmniShadowMappingInfo
{
	mat4	shadowViewProjMatrices[6];
	float	projectionFarPlane;
	vec3	lightPos_world;
};


layout(binding = 0) uniform sampler2D diffuseMap;

layout(binding = 6) uniform samplerCube depthCubeMap;


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec4 fragPos, vec4 lightPos)
{
	// In world space
	vec3 lightToFrag = fragPos.xyz - lightPos.xyz;

	// Next we compute the linear depth of the current fragment from the light POV,
	// which is just the length of the vector going there.
	float currentDepth = length(lightToFrag);


	// now test for shadows

	// Old PCF method

//	float shadow  = 0.0;
//	float bias    = 0.05;
//	float samples = 4.0;
//	float offset  = 0.1;
//	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
//	{
//		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
//		{
//			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
//			{
//				float closestDepth = texture(depthCubeMap, fragToLight + vec3(x, y, z)).r;
//				closestDepth *= projectionFarPlane;   // undo mapping [0;1]
//				if (currentDepth - bias > closestDepth)
//					shadow += 1.0;
//			}
//		}
//	}
//
//	shadow /= (samples * samples * samples);

	// New shiny disk method

	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(cameraPos_world - fragPos);
	float diskRadius = (1.0 + (viewDistance / projectionFarPlane)) / 25.0;

	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(depthCubeMap, lightToFrag + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= projectionFarPlane;   // undo mapping [0;1]

		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}

	shadow /= float(samples);

	return shadow;

}


vec4	ComputeDirectionalLight(int iLight)
{
	// First compute ambient because it will be used no matter what
	vec4 ambient = lightsData[iLight].lightAmbient * materialAmbient;

	// Negate direction vector because we specify the light direction as pointing from the light source.
	// Therefore we negate the light direction to get a direction vector pointing towards the light source.
	vec4 lightDirEye = normalize(view * -lightsData[iLight].lightDirection);

	// Diffuse
	vec3 normalizedNorm = normalize(fs_in.FragEyeNormal); // just to be sure
	float diffuseStrength = max(dot(normalizedNorm, lightDirEye.xyz), 0.0);
	vec4 diffuse = lightsData[iLight].lightDiffuse * materialDiffuse * diffuseStrength;

	// Specular
	float specularStrength = 0.0;
	if (diffuseStrength != 0.0) // Do not produce a specular highlight if the object is back lit.
	{
		vec3 vertToEyeDir = normalize(-fs_in.FragEyePos.xyz); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
		// Compute Blinn-Phong half vector
		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir.xyz);
		specularStrength = pow(max(dot(normalizedNorm, halfwayDir), 0.0), shininess);
	}

	vec4 specular = lightsData[iLight].lightSpecular * materialSpecular * specularStrength;

	// calculate shadow
	float shadow = ShadowCalculation(vec4(fs_in.FragWorldPos, 1.0), lightsData[iLight].lightPosition);

	return ambient + (1.0 - shadow) * (diffuse + specular);
}


vec4	ComputePointLight(int iLight, vec4 lightDirEye, float attenuation)
{
	// First compute ambient because it will be used no matter what

	vec4 ambient = lightsData[iLight].lightAmbient * materialAmbient;

	// Diffuse
	vec3 normalizedNorm = normalize(fs_in.FragEyeNormal); // just to be sure
	float diffuseStrength = max(dot(normalizedNorm, lightDirEye.xyz), 0.0);
	vec4 diffuse = lightsData[iLight].lightDiffuse * materialDiffuse * diffuseStrength;

	// Specular
	float specularStrength = 0.0;
	if (diffuseStrength != 0.0) // Do not produce a specular highlight if the object is back lit.
	{
		vec3 vertToEyeDir = normalize(-fs_in.FragEyePos.xyz); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
		// Compute Blinn-Phong half vector
		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir.xyz);
		specularStrength = pow(max(dot(normalizedNorm, halfwayDir), 0.0), shininess);
	}
	vec4 specular = lightsData[iLight].lightSpecular * materialSpecular * specularStrength;

	// calculate shadow
	float shadow = ShadowCalculation(vec4(fs_in.FragWorldPos, 1.0), lightsData[iLight].lightPosition);

	return (ambient + (1.0 - shadow) * ((diffuse + specular)) * attenuation);
}


vec4	ComputeSpotLight(int iLight, vec4 lightDirEye, float attenuation)
{
	// First compute ambient because it will be used no matter what
	vec4 ambient  = lightsData[iLight].lightAmbient;

	float theta = dot(lightDirEye, normalize(view * -lightsData[iLight].lightDirection)); // -lightDirection : same as above
	float epsilon = lightsData[iLight].lightSpotInnerCutoff - lightsData[iLight].lightSpotOuterCutoff;
	float intensity = clamp((theta - lightsData[iLight].lightSpotOuterCutoff) / epsilon, 0.0, 1.0);

	// Diffuse
	vec3 normalizedNorm = normalize(fs_in.FragEyeNormal); // just to be sure
	float diffuseStrength = max(dot(normalizedNorm, lightDirEye.xyz), 0.0);
	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength;

	// Specular
	float specularStrength = 0.0;
	if (diffuseStrength != 0.0) // Do not produce a specular highlight if the object is back lit.
	{
		vec3 vertToEyeDir = normalize(-fs_in.FragEyePos.xyz); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
		// Compute Blinn-Phong half vector
		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir.xyz);
		specularStrength = pow(max(dot(normalizedNorm, halfwayDir), 0.0), shininess);
	}
	vec4 specular = lightsData[iLight].lightSpecular * specularStrength;

	return ((ambient + diffuse + specular) * attenuation * intensity);
}


void main()
{
	vec4 fragPos4 = vec4(fs_in.FragEyePos, 1.0);

	FragColor = vec4(0.0);

	for (int iLight = 0; iLight < lightsNumber; iLight++)
	{
		if (lightsData[iLight].lightPosition.w == 0) // it's a directional light
		{
			FragColor += ComputeDirectionalLight(iLight);
		}
		else // it's a position light (point or spot) : start calculations
		{
			vec4 lightPosEye = (view * lightsData[iLight].lightPosition);
			vec4 lightDirEye = lightPosEye - fragPos4;

			float distance = length(lightDirEye);
			float attenuation = 1.0 /
			 (lightsData[iLight].lightConstantAttenuation + (lightsData[iLight].lightLinearAttenuation * distance) + (lightsData[iLight].lightQuadraticAttenuation * distance * distance));

			lightDirEye = normalize(lightDirEye);

			if (lightsData[iLight].lightDirection != vec4(0)) // it has position and direction : it's a spot light
			{
				FragColor += ComputeSpotLight(iLight, lightDirEye, attenuation);
			}
			else
			{
				FragColor += ComputePointLight(iLight, lightDirEye, attenuation);
			}
		}
	}

	// Apply the diffuse texture only once all lighting has been computed
	vec4 diffuseMapVal = texture(diffuseMap, fs_in.FragTexCoords);
	FragColor *= diffuseMapVal;

	// apply gamma correction
	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
	FragColor.w = 1.0;
}