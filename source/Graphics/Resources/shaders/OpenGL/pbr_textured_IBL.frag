#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

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
}	Lights;

layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
	vec4	cameraPos;
}	Camera;

layout (std140, binding = 10) uniform ToneMappingParameters
{
	bool	enabled;
	float	exposure;
	bool	useReinhard;
} ToneMapping;




layout(binding = 0) uniform samplerCube irradianceMap;

layout(binding = 2) uniform sampler2D brdfLUT;

layout(binding = 5) uniform samplerCube prefilterMap;


layout(binding = 3) uniform sampler2D albedoMap;

layout(binding = 1) uniform sampler2D normalMap;

layout(binding = 10) uniform sampler2D metallicMap;

layout(binding = 11) uniform sampler2D roughnessMap;

layout(binding = 9) uniform sampler2D aoMap;

in vec3	VertexNormal;

in vec2 vs_texCoords;

in vec3	vs_fragPosWorld;


layout (location = 0) out vec4 FragColor;




// So far, this shader only supports point lights...


const float M_PI = 3.14159265359;


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
//	if (diffuse != 0) // Do not produce a specular highlight if the object is back lit.
//	{
//		vec3 vertToEyeDir = normalize(-vs_fragPosWorld); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
//		// Compute Blinn-Phong half vector
//		vec3 halfwayDir = normalize(lightDirEye.xyz + vertToEyeDir.xyz);
//		specularStrength = pow(max(dot(normalizedNorm, halfwayDir), 0.0), shininess);
//	}
//
//	vec4 specular = lightsData[iLight].lightSpecular * specularStrength;
//	return ambient + diffuse + specular;
//}
//

vec4	ComputePointLight(int iLight, vec4 lightDirEye, vec3 ViewDirEye, float attenuation)
{
	vec4 radiance = Lights.lightsData[iLight].lightDiffuse * attenuation;

	return vec4(0.0);
}


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
//	vec3 vertToEyeDir = normalize(-vs_fragPosWorld); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
//	vec3 reflectDir = reflect(-lightDirEye.xyz, normalizedNorm);
//	float specularStrength = pow(max(dot(vertToEyeDir, reflectDir), 0.0), shininess);
//	vec4 specular = lightsData[iLight].lightSpecular * specularStrength;
//
//	return ((ambient + diffuse + specular) * attenuation * intensity);
//}
//

// Trowbridge-Reitz GGX normal distribution function
float TRGGX_NDF(vec3 Normal, vec3 Halfway, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(Normal, Halfway), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = M_PI * denom * denom;

	return nom / denom; // max(denom, 0.001) prevents divide by zero for roughness=0.0 and NdotH=1.0
}


// Similar to the NDF, the Geometry function takes a material's roughness parameter as input with rougher surfaces having a higher probability of overshadowing microfacets.
// This geometry function is a combination of the GGX and Schlick-Beckmann approximation known as Schlick-GGX.
float SchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}


// Smith method for Schlick-GGX geometry function.
float SmithSchlickGGX_GF(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = SchlickGGX(NdotV, roughness);
	float ggx1 = SchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}


// Fresnel equation using Fresnel-Schlick approximation.
vec3 FresnelSchlick_FE(float cosTheta, vec3 F0)
{
	// In case someday black pixels appear : try cosTheta = min(cosTheta, 1.0)
	// cf. https://learnopengl.com/PBR/Lighting#comment-4581163621
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


// Fresnel equation using Fresnel-Schlick approximation.
// Updated version using a roughness parameter, cf. Sébastien Lagarde https://seblagarde.wordpress.com/2011/08/17/hello-world/
vec3 FresnelSchlickRough_FE(float cosTheta, vec3 F0, float roughness)
{
	// In case someday black pixels appear : try cosTheta = min(cosTheta, 1.0)
	// cf. https://learnopengl.com/PBR/Lighting#comment-4581163621
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


void main()
{
	vec3 FragNormalWorld = normalize(VertexNormal);
	vec3 ViewDirWorld = normalize(Camera.cameraPos.xyz - vs_fragPosWorld);

	// material properties
	// TODO: import albedo textures in SRGB so we can skip the gamma correction here!
	vec3 albedo = texture(albedoMap, vs_texCoords).rgb;
	float metallic = texture(metallicMap, vs_texCoords).r;
	float roughness = texture(roughnessMap, vs_texCoords).r;
	float ao = texture(aoMap, vs_texCoords).r;

	// calculate reflectance at normal incidence for Fresnel Schlick function;
	// Use F0 = 0.04 if dia-electric (like plastic) or use the albedo color as F0 if it's a metal / conductor (metallic workflow)
	vec3 F0 = mix(vec3(0.04), albedo, metallic);

	// lighting using reflectance equation
	vec3 Lo = vec3(0.0);

	for (int iLight = 0; iLight < Lights.lightsNumber; iLight++)
	{
		// calculate per-light radiance
		vec3 L = normalize(Lights.lightsData[iLight].lightPosition.xyz - vs_fragPosWorld);
		vec3 H = normalize(ViewDirWorld + L);
		float distance = length(Lights.lightsData[iLight].lightPosition.xyz - vs_fragPosWorld);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = Lights.lightsData[iLight].lightDiffuse.xyz * attenuation;

		// Cook-Torrance BRDF
		float NDF = TRGGX_NDF(FragNormalWorld, H, roughness);
		float G   = SmithSchlickGGX_GF(FragNormalWorld, ViewDirWorld, L, roughness);
		vec3 F    = FresnelSchlick_FE(clamp(dot(H, ViewDirWorld), 0.0, 1.0), F0);

		vec3 nominator    = NDF * G * F;
		float denominator = 4 * max(dot(FragNormalWorld, ViewDirWorld), 0.0) * max(dot(FragNormalWorld, L), 0.0);
		vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(FragNormalWorld, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo / M_PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}


	// ambient lighting (we now use IBL as the ambient term)
	// we take account of the surface's roughness when calculating the ambient Fresnel response
	// because otherwise, the indirect Fresnel reflection strength looks off on rough non-metal surfaces.
	// (Indirect light follows the same properties of direct light so we expect rougher surfaces to reflect less strongly on the surface edges.)
	vec3 F = FresnelSchlickRough_FE(max(dot(FragNormalWorld, ViewDirWorld), 0.0), F0, roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	vec3 irradiance = texture(irradianceMap, FragNormalWorld).rgb;
	vec3 diffuse      = irradiance * albedo;

	// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
	const float MAX_REFLECTION_LOD = 4.0;
	const vec3 reflectedViewDir = reflect(-ViewDirWorld, FragNormalWorld);

	vec3 prefilteredColor = textureLod(prefilterMap, reflectedViewDir, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf  = texture(brdfLUT, vec2(max(dot(FragNormalWorld, ViewDirWorld), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	vec3 color = ambient + Lo;

	// HDR tonemapping
	if (ToneMapping.enabled)
	{
		if (ToneMapping.useReinhard)
		{
			// Reinhard tone mapping divides the entire HDR color values to LDR color values.
			// Evenly balances out all brightness values onto LDR, but it does tend to slightly favor brighter areas.
			color /= (color + vec3(1.0));
		}
		else // use exposure
		{
			color = vec3(1.0) - exp(-color * ToneMapping.exposure);
		}
	}

	// gamma correct
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);

}