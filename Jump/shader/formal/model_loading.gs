#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 texCoords;
	vec3 texColor;
}gs_in[];

out vec2 TexCoords;
out vec3 TexColor;

uniform float time;

vec3 GetNormal() {
	vec3 a = vec3 (gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3 (gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

vec4 explode (vec4 position, vec3 normal) {
	float magintude = 2.0;
	vec3 direction = normal * ((sin(time + radians(-90.0f)) + 1.0) / 2.0) * magintude;
	return position + vec4(direction, 0.0);
}

void main() {
	vec3 normal = GetNormal();
	TexColor=gs_in[0].texColor;
	gl_Position = explode(gl_in[0].gl_Position, normal); 
	TexCoords = gs_in[0].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position, normal);
	TexCoords = gs_in[1].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position, normal);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();
	EndPrimitive();
}