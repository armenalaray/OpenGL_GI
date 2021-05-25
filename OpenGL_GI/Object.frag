#version 330 core
out vec4 FragColor;

//in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	//FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.0);
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	vec3 result = ambient * objectColor;
	FragColor = vec4(result, 1.0);
}