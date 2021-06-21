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
	//vec4 lightViewPos;
}fsIn;

//uniform sampler2D shadowMap;

uniform samplerCube shadowMap;
uniform vec3 viewPos;
uniform Material mat;
//uniform float specularColor;

//uniform samplerCube skyBox;
//uniform int viewPortHalfWidth;

//uniform Directional_Light dLight;
//#define POINT_LIGHT_COUNT 1

uniform Point_Light pLight;

//uniform Spot_Light sLight;


//uniform float near;
uniform float far;


out vec4 FragColor;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float CalcPointShadows(Point_Light light, vec3 objectPos)
{
	float result = 0.0f;

	vec3 lightDir = objectPos - light.position;
	float fragDepth = length(lightDir);
	
	//FragColor takes zero if not set
	//float bias = max(1.0f * (1.0f - dot(-lightDir,fsIn.normal)), 0.5);
	//float bias = 1.0;
	//float depth = texture(shadowMap, lightDir).r;
	//float linearDepth = depth * far;
	//result += (fragDepth - bias > linearDepth) ? 1.0f : 0.0f;
	//FragColor = vec4(vec3(depth),1.0);

	#if 1
	float bias = max(50.0f * (1.0f - dot(-lightDir,fsIn.normal)), 1.0f);
	//float bias = 10.0f;
	int samples = 20;
	float viewDistance = length(viewPos - objectPos);
	float diskRadius = 0.05f;
	for(int i = 0; i < samples; ++i)
	{
		float depth = texture(shadowMap, lightDir + sampleOffsetDirections[i] * diskRadius).r;
		float linearDepth = depth * far;
		if(fragDepth - bias > linearDepth)
			result += 1.0;
	}
	result/=float(samples);
	#else
	float shadow = 0.0;
	float bias = 0.05;
	float samples = 4.0;
	float offset = 0.1;
	for(float x = -offset; x < offset; x += offset/(samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset/(samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset/(samples * 0.5))
			{
				float depth = texture(shadowMap, lightDir + vec3(x,y,z)).r;
				float linearDepth = depth * far;
				result += (fragDepth - bias > linearDepth) ? 1.0f : 0.0f;
			}
		}
	}
	result /= samples * samples * samples;
	#endif
	return result;
}

vec3 calcPointLightComp(Point_Light light)
{
	vec3 lightDir = vec3(0,0,0);
	float attenuation = 1.0;
	float  spotLightFallOff = 1.0f;

	lightDir = light.position - fsIn.objectPos;
	//NOTE: attenuation
	float d = length(lightDir);
	attenuation = 1.0/(light.c + d*light.l+ d*d*light.q);
	lightDir = normalize(lightDir);

	vec3 norm = normalize(fsIn.normal);
	
	//Diffuse::
	float diffuse = max(dot(norm,lightDir), 0);
	
	//Specular::
	vec3 viewDir = normalize(viewPos - fsIn.objectPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float specular = pow(max(dot(halfDir,norm),0.0),mat.shininess);
	
	//NOTE: we skip alpha!!!
	vec3 diffuseColor = texture(mat.texture_diffuse1, fsIn.texCoords).rgb;
	float specularColor = texture(mat.texture_specular1, fsIn.texCoords).r;
	vec3 emissionColor = texture(mat.emission, fsIn.texCoords).rgb;
	
	vec3 specularLight = spotLightFallOff * specularColor * (specular * light.specular);
	vec3 diffuseLight =  spotLightFallOff * diffuseColor * (diffuse * light.diffuse);
	vec3 ambientLight = diffuseColor * (light.ambient);

	float shadow = CalcPointShadows(light, fsIn.objectPos);
	vec3 result = attenuation * (ambientLight + (1.0f - shadow) * (diffuseLight + specularLight));
	return  result;
}

void main()
{
	vec3 res = vec3(0);
	res += calcPointLightComp(pLight);
	FragColor = vec4(res,1.0);
	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb,vec3(1.0/gamma));
}