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

layout(binding = 7) uniform sampler2D heightMap;



vec2 ComputeParallaxMappingTexCoords(vec2 texCoords, vec3 viewDir)
{
	// TODO : make those constants into a uniform buffer
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	const float heightScale = 0.1;

	// calculate the size of each layer
	const float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	const float layerDepth = 1.0 / numLayers;

	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 parallaxVector = viewDir.xy / viewDir.z * 0.1;
	vec2 deltaTexCoords = parallaxVector / numLayers;

	// get initial values
	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = texture(heightMap, currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of parallaxVector
		currentTexCoords -= deltaTexCoords;
		// get height map value at current texture coordinates
		currentDepthMapValue = texture(heightMap, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// For parallax occlusion mapping : interpolate between the layer we stopped on and the layer before

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	// Some artifacts can appear because parallax mapping can oversample outside the range [0, 1].
	// A trick to solve this issue is to discard the fragment whenever it samples outside the default texture coordinate range.
	// this trick doesn't work on all types of surfaces.
	if (finalTexCoords.x > 1.0 || finalTexCoords.y > 1.0 || finalTexCoords.x < 0.0 || finalTexCoords.y < 0.0)
		discard;

	return finalTexCoords;

//	// number of depth layers
//	const float minLayers = 8.0;
//	const float maxLayers = 32.0;
//	const float numLayers =  mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
//
//	const float heightScale = 0.1;
//
//	// calculate the size of each layer
//	float layerDepth = 1.0 / numLayers;
//
//	// the amount to shift the texture coordinates per layer (from vector P)
//	vec2 parallaxVector = viewDir.xy * heightScale;
//
//	vec2 deltaTexCoords = parallaxVector / numLayers;
//
//	// get initial values
//	vec2  currentTexCoords = texCoords;
//	float heightMapValue = texture(heightMap, currentTexCoords).r;
//
//	// depth of current layer
//	float currentLayerDepth = 0.0;
//
//	while (currentLayerDepth < heightMapValue)
//	{
//		// shift texture coordinates along direction of parallaxVector
//		currentTexCoords -= deltaTexCoords;
//		// get height map value at current texture coordinates
//		heightMapValue = texture(heightMap, currentTexCoords).r;
//		// get depth of next layer
//		currentLayerDepth += layerDepth;
//	}
//
//
//	return currentTexCoords;
}


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
	vec3 viewDirTangent = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec2 parallaxTexCoords = ComputeParallaxMappingTexCoords(fs_in.TexCoords, viewDirTangent);

	// First compute ambient because it will be used no matter what
	// TODO : I think this should be done in main(), not once per light!
	vec4 diffuseMapVal = texture(diffuseMap, parallaxTexCoords);

	vec4 ambient = lightsData[iLight].lightAmbient * diffuseMapVal * materialAmbient;

	 // obtain normal from normal map in range [0,1]
	vec3 normal = texture(normalMap, parallaxTexCoords).rgb;
	// transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

	// Diffuse
	float diffuseStrength = max(dot(normal, lightDirTangent), 0.0);
	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * diffuseMapVal * materialDiffuse;

	// Specular
	float specularStrength = 0.0;
	if (diffuse != 0) // Do not produce a specular highlight if the object is back lit.
	{
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