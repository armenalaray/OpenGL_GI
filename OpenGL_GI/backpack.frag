#version 330 core

struct Material
{
	sampler2D texDiff_0;
};

in vec2 texCoord;
in vec3 normal;
in vec3 objectPos;

uniform Material mat;

out vec4 FragColor;
void main()
{
	FragColor.rgb = texture(mat.texDiff_0, texCoord).rgb;
	FragColor.a = 1.0;
	//FragColor = vec4(1.0);
}