#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct Directional_Light{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Point_Light
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float c;
	float l;
	float q;
};
struct Spot_Light
{
	vec3 position;
	vec3 spotDirection;
	float innerCutoff;
	float outerCutoff;

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

uniform vec3 viewPos;
uniform Material mat;

uniform Directional_Light dLight;
#define POINT_LIGHT_COUNT 4
uniform Point_Light pLight[POINT_LIGHT_COUNT];
uniform Spot_Light sLight;


vec3 calcDirectionalLightComp(Directional_Light light)
{
	float attenuation = 1.0;
	float  spotLightFallOff = 1.0f;
	
	vec3 lightDir = normalize(-light.direction);
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
	
	vec3 specularLight = spotLightFallOff * specularColor * (specular * light.specular);
	vec3 diffuseLight =  spotLightFallOff * diffuseColor * (diffuse * light.diffuse);
	vec3 ambientLight = diffuseColor * (light.ambient);
	
	vec3 result = attenuation * (ambientLight + diffuseLight + specularLight + emissionColor);
	return  result;
}


vec3 calcPointLightComp(Point_Light light)
{
	vec3 lightDir = vec3(0,0,0);
	float attenuation = 1.0;
	float  spotLightFallOff = 1.0f;

	lightDir = light.position - objectPos;
	//NOTE: attenuation
	float d = length(lightDir);
	attenuation = 1.0/(light.c + d*light.l+ d*d*light.q);
	lightDir = normalize(lightDir);


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
	
	vec3 specularLight = spotLightFallOff * specularColor * (specular * light.specular);
	vec3 diffuseLight =  spotLightFallOff * diffuseColor * (diffuse * light.diffuse);
	vec3 ambientLight = diffuseColor * (light.ambient);
	
	vec3 result = attenuation * (ambientLight + diffuseLight + specularLight + emissionColor);
	return  result;
}

vec3 calcSpotLightComp(Spot_Light light)
{
	vec3 lightDir = vec3(0,0,0);
	float attenuation = 1.0;

	lightDir = light.position - objectPos;
	//NOTE: attenuation
	float d = length(lightDir);
	attenuation = 1.0/(light.c + d*light.l+ d*d*light.q);
	lightDir = normalize(lightDir);

	//NOTE: spotLightFallOff
	vec3 spotDir = normalize(-light.spotDirection);
	float theta = dot(lightDir, spotDir);
	float e = light.innerCutoff - light.outerCutoff;
	float spotLightFallOff = clamp((theta - light.outerCutoff)/e,0.0,1.0);

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
	
	vec3 specularLight = spotLightFallOff * specularColor * (specular * light.specular);
	vec3 diffuseLight =  spotLightFallOff * diffuseColor * (diffuse * light.diffuse);
	vec3 ambientLight = diffuseColor * (light.ambient);
	
	vec3 result = attenuation * (ambientLight + diffuseLight + specularLight + emissionColor);
	return  result;
}

void main()
{
	vec3 res = vec3(0);
	res += calcDirectionalLightComp(dLight);
	for(int i = 0; i < 4; ++i)
	{
		res += calcPointLightComp(pLight[i]);
	}
	res += calcSpotLightComp(sLight);
	FragColor = vec4(res,1.0);
	
	
	//FragColor.rgb = lightDir;
	//FragColor.a = 1.0f;
	//NOTE: glsl will accept negative values and treat it as zero!!!!!!
	//FragColor.rgb = objectPos;
	//FragColor.a = 1.0f;
}