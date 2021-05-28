#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct Light{
	vec4 lightVector;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float c;
	float l;
	float q;
};

out vec4 FragColor;

//in vec3 ourColor;

in vec2 texCoord;
in vec3 objectPos;
in vec3 normal;

uniform vec3 lightColor;
uniform vec3 viewPos;

//uniform Directional_Light light;
uniform Light light;
uniform Material mat;

void main()
{
	//FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.0);
	vec3 lightDir = vec3(0,0,0);
	float attenuation = 1.0;
	if(light.lightVector.w == 0.0)
	{
		//NOTE: directional light
		//NOTE:We are specting that the light direction to come from the light source.
		lightDir = normalize(-light.lightVector.xyz);
	}
	else
	{
		//NOTE: spot light
		lightDir = light.lightVector.xyz - objectPos;
		float d = length(lightDir);
		attenuation = 1.0/(light.c + d*light.l+ d*d*light.q);
		lightDir = normalize(lightDir);
	}

	vec3 norm = normalize(normal);

	//Diffuse::
	float diffuse = max(dot(norm,lightDir), 0);

	//Specular::
	vec3 viewDir = normalize(viewPos - objectPos);
	vec3 refDir = reflect(-lightDir, norm);
	//NOTE: the power is the shininess of the highlight
	float specular = pow(max(dot(viewDir,refDir),0.0),mat.shininess);

	//NOTE: we skip alpha!!!
	vec3 diffuseColor = texture(mat.diffuse, texCoord).rgb;
	vec3 specularColor = texture(mat.specular, texCoord).rgb;
	vec3 emissionColor = texture(mat.emission, texCoord).rgb;

	vec3 specularLight = specularColor * (specular * light.specular);
	vec3 diffuseLight =  diffuseColor * (diffuse * light.diffuse);
	vec3 ambientLight = diffuseColor * (light.ambient);
	
	vec3 result = attenuation * (ambientLight + diffuseLight + specularLight + emissionColor);
	FragColor = vec4(result, 1.0);

	//FragColor.rgb = lightDir;
	//FragColor.a = 1.0f;
	//NOTE: glsl will accept negative values and treat it as zero!!!!!!
	//FragColor.rgb = objectPos;
	//FragColor.a = 1.0f;
}