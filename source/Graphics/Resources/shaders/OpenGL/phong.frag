#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (std140, binding = 1) uniform LightCastersData
{
	vec4	lightPosition;
	vec4	lightAmbient;
	vec4	lightDiffuse;
	vec4	lightSpecular;
};

layout (std140, binding = 2) uniform CameraMatrices
{
	mat4	view;
	mat4	projection;
	mat4	viewProjection;
};

layout (std140, binding = 3) uniform PhongMaterial
{
	vec4	materialAmbient;
	vec4	materialDiffuse;
	vec4	materialSpecular;
	float	shininess;
};

in vec3	vs_normal;

in vec3	vs_fragPosEye;

out vec4	FragColor;


void main()
{
	vec4 ambient  = lightAmbient * materialAmbient;

	vec3 normalizedNorm = normalize(vs_normal); // just to be sure
	vec4 dirToLight = (view * lightPosition) - vec4(vs_fragPosEye, 1.0);
	vec4 lightDirEye = normalize(dirToLight);
	float diffuseStrength = max(dot(normalizedNorm, lightDirEye.xyz), 0.0);
	vec4 diffuse = lightDiffuse * diffuseStrength * materialDiffuse;

	vec3 vertToEyeDir = normalize(-vs_fragPosEye); // formula is eye pos - vertex pos but in eye space, eye is at (0, 0, 0) !
	vec3 reflectDir = reflect(-lightDirEye.xyz, normalizedNorm);
	float specularStrength = pow(max(dot(vertToEyeDir, reflectDir), 0.0), shininess);
	vec4 specular = lightSpecular * specularStrength * materialSpecular;

	FragColor = ambient + diffuse + specular;
}