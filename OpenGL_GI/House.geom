#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

//interface block
in VSBLOCK{
	vec3 color;
}gsIn[];

//Normal in
//in vec3 color[];
out vec3 fColor;

vec3 getNormal()
{
	vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
	vec3 normal = normalize(cross(a,b));
	return normal;
}

vec4 explode()
{
	
}

void main()
{
	vec4 p = gl_in[0].gl_Position;
	fColor =  gsIn[0].color;
	gl_Position = p + vec4(-0.2,-0.2,0.0,0.0); //bottom-left
	EmitVertex();
	gl_Position = p + vec4(0.2,-0.2,0.0,0.0); //bottom-right
	EmitVertex();
	gl_Position = p + vec4(-0.2,0.2,0.0,0.0); //top-left
	EmitVertex();
	gl_Position = p + vec4(0.2,0.2,0.0,0.0); //top-right
	EmitVertex();
	gl_Position = p + vec4(0.0,0.4,0.0,0.0); //bottom-left
	fColor =  vec3(1.0,1.0,1.0);
	EmitVertex();
	EndPrimitive();
}


//#version 330 core
//
//layout (points) in;
//layout (points, max_vertices = 1) out;
//
//void main()
//{
//	gl_Position = gl_in[0].gl_Position;
//	EmitVertex();
//	EndPrimitive();
//}	