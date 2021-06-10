#version 330 core

layout (triangles) in;

//NOTE: they are 6 because they are 3 line segments with 2 vertices each.
layout (line_strip, max_vertices = 6) out;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

in VSBLOCK
{
	vec3 normal;
	vec2 texCoords;
}gsIn[];

void generateLine(int index)
{
	float Mag = 0.4;
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = projection * (gl_in[index].gl_Position + vec4(gsIn[index].normal,0) * Mag);
	EmitVertex();
	EndPrimitive();
}

void main()
{
	generateLine(0);
	generateLine(1);
	generateLine(2);
}