#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

in vec3 vs_fragPosWorld;

out vec4 FragColor;


layout(binding = 0) uniform sampler2D equirectangularMap;


const vec2 invAtan = vec2(0.1591, 0.3183);

// Computes a conversion from spherical coordinates to cartesian space.
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(vs_fragPosWorld));
    vec3 color = texture(equirectangularMap, uv).rgb;

    FragColor = vec4(color, 1.0);
}
