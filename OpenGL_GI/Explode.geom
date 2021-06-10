#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VSBLOCK
{
	vec2 texCoords;
}gsIn[];

out vec2 FragTexCoords;

uniform float time;

vec3 getNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 normal = normalize(cross(a,b));
	return normal;
}

vec4 explode(vec4 p, vec3 n)
{
	float mag = 2.0;
	vec3 dir = n * ((sin(time) + 1.0) / 2.0) * mag;
	return p + vec4(dir,0.0);
}


void main()
{
	//NOTE: We are just passing the texcoords to fragment shader as is.
	vec3 n = getNormal();
	gl_Position = explode(gl_in[0].gl_Position, n);
	FragTexCoords = gsIn[0].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position, n);
	FragTexCoords = gsIn[1].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position, n);
	FragTexCoords = gsIn[2].texCoords;
	EmitVertex();
	EndPrimitive();
}