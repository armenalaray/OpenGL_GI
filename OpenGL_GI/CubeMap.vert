#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 texCoord;

void main()
{
	texCoord = aPos;
	vec4 p = projection * view * vec4(texCoord, 1.0);
	gl_Position = p.xyww;
}
