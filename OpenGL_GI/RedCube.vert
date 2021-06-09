#version 330 core

layout (location = 0) in vec3 pIn;
layout (location = 1) in vec2 tIn;

//128 bytes total size uniform block Matrices
layout (std140) uniform Matrices
{                    //  offset:           size:
	mat4 projection; //  0                 4*N*4
	mat4 view;       //  4*N*4             4*N*4
};

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(pIn, 1.0);
}