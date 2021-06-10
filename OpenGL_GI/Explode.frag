#version 330 core

struct Material
{
	sampler2D texDiff_0;
	sampler2D texSpec_0;
	sampler2D emission;
	float shininess;
};

//in VSBLOCK
//{
//	vec2 texCoords;
//}gsIn;

in vec2 FragTexCoords;

uniform Material mat;

out vec4 FragColor;

void main()
{
	FragColor = texture(mat.texDiff_0, FragTexCoords);	
	//FragColor = vec4(1.0,0.0,0.0,1.0);
}