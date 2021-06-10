#version 330 core
layout (location = 0) in vec3 pIn;
layout (location = 1) in vec3 nIn;
layout (location = 2) in vec2 tIn;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out VSBLOCK
{
	vec2 texCoords;
}vsOut;

void main()
{
	vsOut.texCoords = tIn;
	gl_Position = projection * view * model * vec4(pIn, 1.0);
}