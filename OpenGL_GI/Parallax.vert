#version 330 core

layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 textureIn;
layout (location = 3) in vec3 tangentIn;
layout (location = 4) in vec3 bitangentIn;

struct Point_Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 clq;
};

out VSBLOCK
{
	vec3 objectPos;
	vec3 normal;
	vec2 texCoords;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
	//vec4 lightViewPos;
}vsOut;

layout (std140) uniform Matrices
{
	//16N
	mat4 projection;
	//16N
	mat4 view;
	//mat4 lightViewOrthoMatrix;
};

layout (std140) uniform Lights
{
	//20N
	Point_Light pLight;	
};

uniform vec3 viewPos;
uniform mat4 modelMatrix;

void main()
{
	vec4 verWorldP = modelMatrix * vec4(positionIn,1.0);
	
	mat3 nMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 T = normalize(nMatrix * tangentIn);
	vec3 B = normalize(nMatrix * bitangentIn);
	vec3 N = normalize(nMatrix * normalIn);
	
	//Gram-Schmidt process
	T = normalize(T - dot(T,N) * N);
	B = cross(N,T);

	mat3 TBN = mat3(T,B,N);
	mat3 TBNInverse = transpose(TBN);

	vsOut.objectPos = verWorldP.xyz;
	vsOut.normal = N;
	vsOut.texCoords = textureIn;
	vsOut.tangentLightPos = TBNInverse * pLight.position;
	vsOut.tangentViewPos = TBNInverse * viewPos;
	vsOut.tangentFragPos = TBNInverse * vec3(verWorldP);

	gl_Position = projection * view * verWorldP;
}