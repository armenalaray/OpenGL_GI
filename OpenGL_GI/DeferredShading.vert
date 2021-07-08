#version 330 core

layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec2 textureIn;

out vec2 texCoords;

void main()
{
	texCoords = textureIn;
	gl_Position = vec4(positionIn,1.0);
}