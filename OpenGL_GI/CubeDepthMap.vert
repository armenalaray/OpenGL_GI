#version 330 core
/*
NOTE: we will assume that here we pass all the vertices that we have in the buffer
*/
layout (location = 0) in vec3 pIn;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(pIn, 1.0);
}