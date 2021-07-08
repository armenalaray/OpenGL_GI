#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 texCoords;
uniform sampler2D tex;

void main()
{
	FragColor = vec4(texture(tex,texCoords).rgb, 1.0);
}