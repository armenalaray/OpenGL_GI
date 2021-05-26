#version 330 core
out vec4 FragColor;

//in vec3 ourColor;
//in vec2 TexCoord;

//NOTE: I calc objectPos in vertex shader in world coordinates
in vec3 objectPos;
in vec3 normal;

//uniform sampler2D texture0;
//uniform sampler2D texture1;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
	//FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.0);

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - objectPos);

	float cosTheta = max(dot(norm,lightDir), 0);
	vec3 diffuseLight = cosTheta * lightColor;

	float specularStrenght = 0.5;

	vec3 viewDir = normalize(viewPos - objectPos);
	vec3 refDir = reflect(-lightDir, norm);
	//NOTE: the power is the shininess of the highlight
	float spec = pow(max(dot(viewDir,refDir),0.0),128);
	vec3 specular = specularStrenght * spec * lightColor;

	float ambientStrength = 0.1;
	vec3 ambientLight = ambientStrength * lightColor;
	
	vec3 result = (ambientLight + diffuseLight + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}