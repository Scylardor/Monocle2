#version 330 core

layout (std140) uniform LightCastersData
{
	vec4	lightAmbient;
	vec4	lightDiffuse;
	//vec4	lightSpecular;
	vec4	lightPosition;
};

layout (std140) uniform PhongMaterial
{
	vec4	materialDiffuse;
};

in vec3	vs_normal;

in vec3	vs_fragPosWorld;

out vec4	FragColor;


void main()
{
	vec3 fragNormal = normalize(vs_normal);
	vec3 dirToLight = normalize(lightPosition.xyz - vs_fragPosWorld);

	float diffuseStrength = max(dot(fragNormal, dirToLight), 0.0);
	vec4 diffuse = diffuseStrength * lightDiffuse;

	FragColor = (lightAmbient + diffuse) * materialDiffuse;
}