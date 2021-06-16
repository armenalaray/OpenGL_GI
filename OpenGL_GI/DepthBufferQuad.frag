#version 330 core

in vec2 texCoords;

uniform sampler2D screenTexture;

out vec4 fragColor;

uniform float near;
uniform float far;

float LinearDepth(float depth)
{
	float zn = depth * 2.0f - 1.0;
	float ze = (2 * far * near) / (zn * (far - near) - far - near);
	return ze;
}

void main()
{
	float depthValue = texture(screenTexture, texCoords).r;
	//float color = LinearDepth(depthValue) / far;
	float color = depthValue;
	fragColor = vec4(vec3(color),1.0);
	//fragColor = vec4(1.0,0.0,0.0,1.0);
}


