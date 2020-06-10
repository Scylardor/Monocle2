#version 420 core
// Require version 420 to be able to use "binding = ..." extension.

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec2 texCoords;
} gs_in[];

out vec2 v_texCoords;

layout (std140, binding = 0) uniform FrameTime
{
	float sinFrameTime;
};


layout (std140, binding = 4) uniform ObjectMatrices
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewProjection;
	mat3 normalMatrix;
};



vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;
	vec3 direction = normal * sinFrameTime * magnitude;

	return position + vec4(direction, 0.0);
}


vec3 GetNormal()
{
	vec3 v1 = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);

	vec3 v2 = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

	return normalize(cross(v2, v1));

}

void main() {
	vec3 normal = GetNormal();

	gl_Position = modelViewProjection * explode(gl_in[0].gl_Position, normal);
	v_texCoords = gs_in[0].texCoords;
	EmitVertex();

	gl_Position = modelViewProjection * explode(gl_in[1].gl_Position, normal);
	v_texCoords = gs_in[1].texCoords;
	EmitVertex();

	gl_Position = modelViewProjection * explode(gl_in[2].gl_Position, normal);
	v_texCoords = gs_in[2].texCoords;
	EmitVertex();

	EndPrimitive();
}