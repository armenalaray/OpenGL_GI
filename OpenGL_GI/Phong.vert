#version 330 core

layout (location = 0) in vec3 pIn;
layout (location = 1) in vec3 nIn;
layout (location = 2) in vec2 tIn;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
	mat4 lightViewOrthoMatrix;
};

out VSBLOCK
{
	vec3 objectWorldP;
	vec3 normal;
	vec2 texCoords;
	vec4 lightViewPos;
}vsOut;

uniform mat4 modelMatrix;

void main()
{
	vec4 verWorldP = modelMatrix * vec4(pIn,1.0);
	//NOTE: no aplicaban la rotación de la vista!!!!
	mat3 nMatrix = transpose(inverse(mat3(modelMatrix)));
	vsOut.normal = normalize(nMatrix * nIn);
	vsOut.texCoords = tIn;
	vsOut.lightViewPos = lightViewOrthoMatrix * verWorldP;
	vsOut.objectWorldP = verWorldP.xyz;
	gl_Position = projection * view * verWorldP;
}