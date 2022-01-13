#version 420 core
// Require version 420 to be able to use "binding = ..." extension.
//
//#define LIGHTS_NBR 16
//
//struct LightData
//{
//	vec4	lightPosition;
//	vec4	lightDirection;
//	vec4	lightAmbient;
//	vec4	lightDiffuse;
//	vec4	lightSpecular;
//	float	lightConstantAttenuation;
//	float	lightLinearAttenuation;
//	float	lightQuadraticAttenuation;
//	float	lightSpotInnerCutoff;
//	float	lightSpotOuterCutoff;
//};
//
//layout (std140, binding = 1) uniform LightCastersData
//{
//	uint		lightsNumber;
//	LightData	lightsData[LIGHTS_NBR];
//};
//

//
//layout (std140, binding = 3) uniform PhongMaterial
//{
//	vec4	materialAmbient;
//	vec4	materialDiffuse;
//	vec4	materialSpecular;
//	float	shininess;
//};

layout (binding = 0) uniform sampler2D T_Diffuse;


in VS_OUT {
	vec3	PosVS;
	vec3	NormalVS;
	vec2	UV0;
} fs_in;

out vec4	FragColor;



//vec4	ComputeDirectionalLight(int iLight)
//{
//	// First compute ambient because it will be used no matter what
//	vec4 diffuseMapVal = texture(diffuseMap, vs_texCoords);
//	vec4 ambient  = lightsData[iLight].lightAmbient * diffuseMapVal;
//
//	// Negate direction vector because we specify the light direction as pointing from the light source.
//	// Therefore we negate the light direction to get a direction vector pointing towards the light source.
//	vec4 lightDirEye = normalize(view * -lightsData[iLight].lightDirection);
//
//	// Diffuse
//	vec3 normalizedNorm = normalize(vs_normal); // just to be sure
//	float diffuseStrength = max(dot(normalizedNorm, lightDirEye.xyz), 0.0);
//	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * diffuseMapVal;
//
//	// Specular
//	float specularStrength = 0.0;
//	if (diffuse != vec4(0)) // Do not produce a specular highlight if the object is back lit.
//	{
//		vec3 vertToEyeDir = normalize(-vs_fragPosEye); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
//		// Compute Blinn-Phong half vector
//		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir.xyz);
//		specularStrength = pow(max(dot(normalizedNorm, halfwayDir), 0.0), shininess);
//	}
//
//	vec4 specular = lightsData[iLight].lightSpecular * specularStrength;
//	return ambient + diffuse + specular;
//}
//
//
//vec4	ComputePointLight(int iLight, vec4 lightDirEye, float attenuation)
//{
//	// First compute ambient because it will be used no matter what
//	vec4 diffuseMapVal = texture(diffuseMap, vs_texCoords);
//	vec4 ambient  = lightsData[iLight].lightAmbient * diffuseMapVal;
//
//	// Diffuse
//	vec3 normalizedNorm = normalize(vs_normal); // just to be sure
//	float diffuseStrength = max(dot(normalizedNorm, lightDirEye.xyz), 0.0);
//	vec4 diffuse = lightsData[iLight].lightDiffuse * diffuseStrength * diffuseMapVal;
//
//	// Specular
//	float specularStrength = 0.0;
//	if (diffuse != vec4(0)) // Do not produce a specular highlight if the object is back lit.
//	{
//		vec3 vertToEyeDir = normalize(-vs_fragPosEye); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
//		// Compute Blinn-Phong half vector
//		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir.xyz);
//		specularStrength = pow(max(dot(normalizedNorm, halfwayDir), 0.0), shininess);
//	}
//	vec4 specular = lightsData[iLight].lightSpecular * specularStrength;
//
//	return ((ambient + diffuse + specular) * attenuation);
//}
//
//
//vec4	ComputeSpotLight(int iLight, vec4 lightDirEye, float attenuation)
//{
//	// First compute ambient because it will be used no matter what
//	vec4 diffuseMapVal = texture(diffuseMap, vs_texCoords);
//	vec4 ambient  = lightsData[iLight].lightAmbient * diffuseMapVal;
//
//	float theta = dot(lightDirEye, normalize(view * -lightsData[iLight].lightDirection)); // -lightDirection : same as above
//	float epsilon = lightsData[iLight].lightSpotInnerCutoff - lightsData[iLight].lightSpotOuterCutoff;
//	float intensity = clamp((theta - lightsData[iLight].lightSpotOuterCutoff) / epsilon, 0.0, 1.0);
//
//	// Diffuse
//	vec3 normalizedNorm = normalize(vs_normal); // just to be sure
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
//}
//



void main()
{
	//vec4 fragPos4 = vec4(vs_fragPosEye, 1.0);
	//
	//for (int iLight = 0; iLight < lightsNumber; iLight++)
	//{
	//	if (lightsData[iLight].lightPosition.w == 0) // it's a directional light
	//	{
	//		FragColor += ComputeDirectionalLight(iLight);
	//	}
	//	else // it's a position light (point or spot) : start calculations
	//	{
	//		vec4 lightPosEye = (view * lightsData[iLight].lightPosition);
	//		vec4 lightDirEye = lightPosEye - fragPos4;
	//
	//		float distance = length(lightDirEye);
	//		float attenuation = 1.0 /
	//		 (lightsData[iLight].lightConstantAttenuation + (lightsData[iLight].lightLinearAttenuation * distance) + (lightsData[iLight].lightQuadraticAttenuation * distance * distance));
	//
	//		lightDirEye = normalize(lightDirEye);
	//
	//		if (lightsData[iLight].lightDirection != vec4(0)) // it has position and direction : it's a spot light
	//		{
	//			FragColor += ComputeSpotLight(iLight, lightDirEye, attenuation);
	//		}
	//		else
	//		{
	//			FragColor += ComputePointLight(iLight, lightDirEye, attenuation);
	//		}
	//	}
	//}
	//
	//FragColor.w = 1.0;

	FragColor = texture(T_Diffuse, fs_in.UV0);
}