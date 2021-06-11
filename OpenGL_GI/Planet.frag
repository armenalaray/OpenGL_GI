#version 330 core

in VSBLOCK
{
	vec3 normal;
	vec2 texCoords;
}objectIn;


uniform sampler2D texture_diffuse1;

out vec4 FragColor;

void main()
{
	FragColor = texture(texture_diffuse1, objectIn.texCoords);	
	//FragColor = vec4(1.0,0.0,0.0,1.0);
}