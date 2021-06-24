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

struct Point_Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 clq;
};

in VSBLOCK
{
	vec3 objectPos;
	vec3 normal;
	vec2 texCoords;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
	//vec4 lightViewPos;
}fsIn;


layout (std140) uniform Lights
{
	Point_Light pLight;	
};

uniform samplerCube shadowMap;
uniform Material mat;
uniform float far;
uniform float specularColor;

uniform float height_scale;

out vec4 FragColor;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

//float CalcPointShadows(Point_Light light, vec3 objectPos)
//{
//	float result = 0.0f;
//
//	vec3 lightDir = objectPos - light.position;
//	float fragDepth = length(lightDir);
//	
//	//FragColor takes zero if not set
//	//float bias = max(1.0f * (1.0f - dot(-lightDir,fsIn.normal)), 0.5);
//	//float bias = 1.0;
//	//float depth = texture(shadowMap, lightDir).r;
//	//float linearDepth = depth * far;
//	//result += (fragDepth - bias > linearDepth) ? 1.0f : 0.0f;
//	//FragColor = vec4(vec3(depth),1.0);
//
//	#if 1
//	float bias = max(50.0f * (1.0f - dot(-lightDir,fsIn.normal)), 1.0f);
//	//float bias = 10.0f;
//	int samples = 20;
//	float viewDistance = length(viewPos - objectPos);
//	float diskRadius = 0.05f;
//	for(int i = 0; i < samples; ++i)
//	{
//		float depth = texture(shadowMap, lightDir + sampleOffsetDirections[i] * diskRadius).r;
//		float linearDepth = depth * far;
//		if(fragDepth - bias > linearDepth)
//			result += 1.0;
//	}
//	result/=float(samples);
//	#else
//	float shadow = 0.0;
//	float bias = 0.05;
//	float samples = 4.0;
//	float offset = 0.1;
//	for(float x = -offset; x < offset; x += offset/(samples * 0.5))
//	{
//		for(float y = -offset; y < offset; y += offset/(samples * 0.5))
//		{
//			for(float z = -offset; z < offset; z += offset/(samples * 0.5))
//			{
//				float depth = texture(shadowMap, lightDir + vec3(x,y,z)).r;
//				float linearDepth = depth * far;
//				result += (fragDepth - bias > linearDepth) ? 1.0f : 0.0f;
//			}
//		}
//	}
//	result /= samples * samples * samples;
//	#endif
//	return result;
//}


vec2 ParallaxMapping(vec3 tangentViewDir, vec2 texCoords)
{
	float height = texture(mat.texture_height1, texCoords).r;
	vec2 p = tangentViewDir.xy * height * height_scale;
//	vec2 p = tangentViewDir.xy / tangentViewDir.z * height * height_scale;
	return texCoords - p;
}

vec2 SteepParallaxMapping(vec3 tangentViewDir, vec2 texCoords)
{
	int layerCount = 100;
	float layerDepth = 1.0f / layerCount;
	float currentLayerDepth = 0.0f;

	//NOTE: this is arbitrary we control how much we move the whole p with height_scale!!!
	vec2 p = tangentViewDir.xy * height_scale;
	vec2 deltaTexCoords = p / layerCount;

	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(mat.texture_height1, currentTexCoords).r;
	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(mat.texture_height1, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}

	return currentTexCoords;
}

vec3 calcPointLightComp(Point_Light light)
{
	vec3 tangentlightDir = vec3(0,0,0);
	float attenuation = 1.0;
	float  spotLightFallOff = 1.0f;

	tangentlightDir = fsIn.tangentLightPos - fsIn.tangentFragPos;
	vec3 tangentViewDir = normalize(fsIn.tangentViewPos - fsIn.tangentFragPos);

//	vec2 texCoords = ParallaxMapping(tangentViewDir, fsIn.texCoords);
	vec2 texCoords = SteepParallaxMapping(tangentViewDir, fsIn.texCoords);
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0)
		discard;

	//NOTE: attenuation
	float d = length(tangentlightDir);
	attenuation = 1.0/(light.clq.x + d*light.clq.y + d*d*light.clq.z);
	tangentlightDir = normalize(tangentlightDir);

	vec3 tangentNorm = texture(mat.texture_normal1, texCoords).rgb;
	//FragColor = vec4(tangentNorm,1.0);
	tangentNorm = tangentNorm * 2.0f - 1.0f;

	//Diffuse::
	float diffuse = max(dot(tangentNorm,tangentlightDir), 0);
	
	//Specular::
	vec3 tangentHalfDir = normalize(tangentlightDir + tangentViewDir);
	float specular = pow(max(dot(tangentHalfDir, tangentNorm),0.0), mat.shininess);
	
	//NOTE: we skip alpha!!!
	vec3 diffuseColor = texture(mat.texture_diffuse1, texCoords).rgb;
//	float specularColor = texture(mat.texture_specular1, texCoords).r;
	vec3 emissionColor = texture(mat.emission, texCoords).rgb;
	
	vec3 specularLight = spotLightFallOff * specularColor * (specular * light.specular);
	vec3 diffuseLight =  spotLightFallOff * diffuseColor * (diffuse * light.diffuse);
	vec3 ambientLight = diffuseColor * (light.ambient);

	//float shadow = CalcPointShadows(light, fsIn.tangentFragPos);
	float shadow = 0.0f;
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
	//FragColor = vec4(pLight.clq,1.0);
}