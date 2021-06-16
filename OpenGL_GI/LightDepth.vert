#version 330 core

layout (location = 0) in vec3 pIn;
layout (location = 1) in vec3 nIn;
layout (location = 2) in vec3 tIn;

uniform mat4 lightViewOrthoMatrix;
uniform mat4 modelMatrix;

void main()
{
	gl_Position = lightViewOrthoMatrix  * modelMatrix * vec4(pIn,1.0);
}