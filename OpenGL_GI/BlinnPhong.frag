#version 330 core
//layout (depth_greater) out float gl_FragDepth;


//TODO: Make a preprocessor to have .h files!!!
struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
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

in VSBLOCK
{
    vec3 objectPos;
	vec3 normal;
	vec2 texCoords;
}fsIn;


uniform vec3 viewPos;
uniform Material mat;
uniform float specularColor;

//uniform samplerCube skyBox;
//uniform int viewPortHalfWidth;

uniform Directional_Light dLight;
//#define POINT_LIGHT_COUNT 1
//uniform Point_Light pLight[POINT_LIGHT_COUNT];
//uniform Spot_Light sLight;


//uniform float near;
//uniform float far;


out vec4 FragColor;


vec3 calcDirectionalLightComp(Directional_Light light)
{
	float attenuation = 1.0;
	float  spotLightFallOff = 1.0f;
	
	vec3 lightDir = normalize(-light.direction);
	vec3 norm = normalize(fsIn.normal);
	
	//Diffuse::
	float diffuse = max(dot(norm,lightDir), 0);
	
	//Specular::
	vec3 viewDir = normalize(viewPos - fsIn.objectPos);
	vec3 refDir = reflect(-lightDir, norm);
	//NOTE: the power is the shininess of the highlight
	float specular = pow(max(dot(viewDir,refDir),0.0),mat.shininess);
	
	vec4 texDiffuse = texture(mat.texture_diffuse1, fsIn.texCoords);
	if(texDiffuse.a < 0.1)
	{
		discard;
	}

	vec3 diffuseColor = texDiffuse.rgb;
	//float specularColor = texture(mat.texture_specular1, texCoord).r;
	vec3 emissionColor = texture(mat.emission, fsIn.texCoords).rgb;
	
	vec3 specularLight = spotLightFallOff * specularColor * (specular * light.specular);
	vec3 diffuseLight =  spotLightFallOff * diffuseColor * (diffuse * light.diffuse);
	vec3 ambientLight = diffuseColor * (light.ambient);
	
	vec3 result = attenuation * (ambientLight + diffuseLight + specularLight);
	return  result;
}

void main()
{
	vec3 res = vec3(0);
	res += calcDirectionalLightComp(dLight);
	//for(int i = 0; i < POINT_LIGHT_COUNT; ++i)
	//{
	//	res += calcPointLightComp(pLight[i]);
	//}
	//res += calcSpotLightComp(sLight);
	FragColor = vec4(res,1.0);
}