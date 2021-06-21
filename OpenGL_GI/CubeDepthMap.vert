#version 330 core

layout (location = 0) in vec3 pIn;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = modelMatrix * vec4(pIn, 1.0);
}