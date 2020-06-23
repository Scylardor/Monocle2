#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

#define LIGHTS_NBR 16

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos; // so far, manage only a single light...
	vec3 TangentViewPos;
	vec3 TangentFragPos;
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

layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};


layout(binding = 0) uniform sampler2D diffuseMap;

layout(binding = 1) uniform sampler2D normalMap;




vec4	ComputeDirectionalLight(int iLight)
{
	// Directional light not implemented for normal mapping
	return vec4(0.0);
	// First compute ambient because it will be used no matter what
//	vec4 diffuseMapVal = texture(diffuseMap, vs_texCoords);
//	vec4 ambient  = lightsData[iLight].lightAmbient * diffuseMapVal;
//
//	// Negate direction vector because we specify the light direction as pointing from the light source.
//	// Therefore we negate the light direction to get a direction vector pointing towards the light source.
//	vec4 lightDirEye = normalize(view * -lightsData[iLight].lightDirection);
//
//	// Diffuse
//	// obtain normal from normal map in range [0,1]
//	vec3 normalizedNorm = texture(normalMap, vs_texCoords).rgb;
//	// transform normal vector to range [-1,1], then normalize just to be sure
//	normalizedNorm = normalize(normalizedNorm * 2.0 - 1.0);
//
//	float diffuseStrength = max(dot(normalizedNorm, lightDirEye.xyz), 0.0);
//	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * diffuseMapVal;
//
//	// Specular
//	float specularStrength = 0.0;
//	if (diffuse != 0) // Do not produce a specular highlight if the object is back lit.
//	{
//		vec3 vertToEyeDir = normalize(-vs_fragPosEye); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
//		// Compute Blinn-Phong half vector
//		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir.xyz);
//		specularStrength = pow(max(dot(normalizedNorm, halfwayDir), 0.0), shininess);
//	}
//
//	vec4 specular = lightsData[iLight].lightSpecular * specularStrength;
//	return ambient + diffuse + specular;
}


vec4	ComputePointLight(int iLight, vec3 lightDirTangent, float attenuation)
{
	// First compute ambient because it will be used no matter what
	// TODO : I think this should be done in main(), not once per light!
	vec4 diffuseMapVal = texture(diffuseMap, fs_in.TexCoords);

	vec4 ambient = lightsData[iLight].lightAmbient * diffuseMapVal * materialAmbient;

	 // obtain normal from normal map in range [0,1]
	vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
	// transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

	// Diffuse
	float diffuseStrength = max(dot(normal, lightDirTangent), 0.0);
	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * diffuseMapVal * materialDiffuse;

	// Specular
	float specularStrength = 0.0;
	if (diffuse != 0) // Do not produce a specular highlight if the object is back lit.
	{
		vec3 viewDirTangent = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

		// Compute Blinn-Phong half vector
		vec3 halfwayDir = normalize(lightDirTangent + viewDirTangent);
		specularStrength = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	}

	vec4 specular = lightsData[iLight].lightSpecular * specularStrength * materialSpecular;

	return ((ambient + diffuse + specular) * attenuation);
}


vec4	ComputeSpotLight(int iLight, vec3 lightDirTangent, float attenuation)
{
	// Spot light not implemented for normal mapping
	return vec4(0.0);
	// First compute ambient because it will be used no matter what
//	vec4 diffuseMapVal = texture(diffuseMap, vs_texCoords);
//	vec4 ambient  = lightsData[iLight].lightAmbient * diffuseMapVal;
//
//	float theta = dot(lightDirEye, normalize(view * -lightsData[iLight].lightDirection)); // -lightDirection : same as above
//	float epsilon = lightsData[iLight].lightSpotInnerCutoff - lightsData[iLight].lightSpotOuterCutoff;
//	float intensity = clamp((theta - lightsData[iLight].lightSpotOuterCutoff) / epsilon, 0.0, 1.0);
//
//	// Diffuse
//	// obtain normal from normal map in range [0,1]
//	vec3 normalizedNorm = texture(normalMap, vs_texCoords).rgb;
//	// transform normal vector to range [-1,1], then normalize just to be sure
//	normalizedNorm = normalize(normalizedNorm * 2.0 - 1.0);
//
//	float diffuseStrength = max(dot(normalizedNorm, lightDirEye.xyz), 0.0);
//	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * diffuseMapVal;
//
//	// Specular
//	vec3 vertToEyeDir = normalize(-vs_fragPosEye); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
//	vec3 reflectDir = reflect(-lightDirEye.xyz, normalizedNorm);
//	float specularStrength = pow(max(dot(vertToEyeDir, reflectDir), 0.0), shininess);
//	vec4 specular = lightsData[iLight].lightSpecular * specularStrength;
//
//	return ((ambient + diffuse + specular) * attenuation * intensity);
}



// TODO : this code does not work for multiple lights anymore.

void main()
{
	FragColor = vec4(0.0);

	for (int iLight = 0; iLight < lightsNumber; iLight++)
	{
		if (lightsData[iLight].lightPosition.w == 0) // it's a directional light
		{
			FragColor += ComputeDirectionalLight(iLight);
		}
		else // it's a position light (point or spot) : start calculations
		{
			vec3 lightPosTangent = fs_in.TangentLightPos;
			vec3 lightDirTangent = lightPosTangent - fs_in.TangentFragPos;

			float lightDistance = length(lightDirTangent);
			float attenuation = 1.0 /
			 (lightsData[iLight].lightConstantAttenuation + (lightsData[iLight].lightLinearAttenuation * lightDistance) + (lightsData[iLight].lightQuadraticAttenuation * lightDistance * lightDistance));

			lightDirTangent = normalize(lightDirTangent);

			if (lightsData[iLight].lightDirection != vec4(0)) // it has position and direction : it's a spot light
			{
				FragColor += ComputeSpotLight(iLight, lightDirTangent, attenuation);
			}
			else
			{
				FragColor += ComputePointLight(iLight, lightDirTangent, attenuation);
			}
		}
	}

	FragColor.w = 1.0;
}