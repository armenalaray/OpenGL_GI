#version 330 core
out vec4 FragColor;

struct Material
{
	sampler2D texDiff_0;
};

in vec2 texCoord;
//uniform sampler2D t0;
uniform Material mat;

void main()
{
	FragColor = texture(mat.texDiff_0, texCoord);
}