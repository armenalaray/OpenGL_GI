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
	vec3 objectWorldP;
	vec3 normal;
	vec2 texCoords;
	vec4 lightViewPos;
}fsIn;

uniform sampler2D shadowMap;
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

float CalcShadows()
{
	float result = 0.0f;

	//NOTE: account for perspective division, if any
	vec3 ndcCoord = fsIn.lightViewPos.xyz / fsIn.lightViewPos.w; 
	vec3 normCoord = (ndcCoord + vec3(1.0f)) * 0.5f;
	float fragDepth  = normCoord.z;

	float bias = max(0.5 * (1.0f - dot(fsIn.normal, -dLight.direction)), 0.005f);
	vec2 texSize = 1.0 / textureSize(shadowMap, 0);
	for(int x =- 1; x <= 1; ++x)
	{
		for(int y =- 1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, normCoord.xy + vec2(x,y) * texSize).r;
			if(fragDepth - bias > pcfDepth) result += 1.0f; else result += 0.0f;
		}
	}

	result /= 9.0;

	//NOTE: we are unshadowing the result if the fragment is outside the light's frustrum far plane .
	if(fragDepth > 1.0) result = 0.0f;
	return result;
}

vec3 calcDirectionalLightCompPhong(Directional_Light light, float shininess)
{
	float attenuation = 1.0;
	float  spotLightFallOff = 1.0f;
	
	vec3 lightDir = normalize(-light.direction);
	vec3 norm = normalize(fsIn.normal);
	
	//Diffuse::
	float diffuse = max(dot(norm,lightDir), 0);
	
	//Specular::
	vec3 viewDir = normalize(viewPos - fsIn.objectWorldP);
	vec3 refDir = reflect(-lightDir, norm);
	//NOTE: the power is the shininess of the highlight
	float specular = pow(max(dot(viewDir,refDir),0.0),shininess);
	
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

vec3 calcDirectionalLightCompBlinnPhong(Directional_Light light, float shininess)
{
	float attenuation = 1.0;
	float  spotLightFallOff = 1.0f;
	
	vec3 lightDir = normalize(-light.direction);
	vec3 norm = normalize(fsIn.normal);
	
	//Diffuse::
	float diffuse = max(dot(norm,lightDir), 0);
	
	//Specular::
	vec3 viewDir = normalize(viewPos - fsIn.objectWorldP);
	vec3 halfDir = normalize(lightDir + viewDir);
	float specular = pow(max(dot(halfDir,fsIn.normal),0.0), shininess);
	
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
	
	float shadow = CalcShadows();

	vec3 result = attenuation * (ambientLight + (1-shadow)*(diffuseLight + specularLight));
	return  result;
}


void main()
{
	vec3 res = vec3(0);
	float myShininess = mat.shininess;
	res += calcDirectionalLightCompBlinnPhong(dLight, myShininess);
	FragColor = vec4(res,1.0);
	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb,vec3(1.0/gamma));
}