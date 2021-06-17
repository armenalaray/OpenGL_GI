#version 330 core
/*
Entonces de todos los vertices generamos otros 5 mas aqui.
*/
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main()
{
	for(int face = 0; face < 6; ++face)
	{
		/*
		gl_Layer contains the selected layer of a multi-layer framebuffer attachment
		*/
		gl_Layer = face;
		for(int i=0; i < 3; ++i)
		{
			FragPos =  gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}