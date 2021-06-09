#version 330 core
layout (location = 0) in vec2 pIn;
layout (location = 1) in vec3 colorIn;

//interface block
out VSBLOCK{
	vec3 color;
}vsOut;

//Normal out
//out vec3 color;

void main()
{
	gl_PointSize = 1.0f;
	gl_Position = vec4(pIn.x, pIn.y, 0.0, 1.0);
	vsOut.color = colorIn;
}