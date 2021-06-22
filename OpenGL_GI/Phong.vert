#version 330 core

layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 textureIn;
layout (location = 3) in vec3 tangentIn;
layout (location = 4) in vec3 bitangentIn;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
	//mat4 lightViewOrthoMatrix;
};

out VSBLOCK
{
	vec3 objectPos;
	vec3 normal;
	vec2 texCoords;
	mat3 TBN;

	//vec4 lightViewPos;
}vsOut;

uniform mat4 modelMatrix;

void main()
{
	vec4 verWorldP = modelMatrix * vec4(positionIn,1.0);
	
	mat3 nMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 T = normalize(nMatrix * tangentIn);
	vec3 B = normalize(nMatrix * bitangentIn);
	vec3 N = normalize(nMatrix * normalIn);
	vsOut.normal = N;
	vsOut.TBN = mat3(T,B,N);
	vsOut.texCoords = textureIn;
	vsOut.objectPos = verWorldP.xyz;

	gl_Position = projection * view * verWorldP;
}