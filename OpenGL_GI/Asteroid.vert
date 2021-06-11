#version 330 core

layout (location = 0) in vec3 pIn;
layout (location = 1) in vec3 nIn;
layout (location = 2) in vec2 tIn;
//layout (location = 3) in mat4 modelMatrix;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out VSBLOCK
{
	vec3 normal;
	vec2 texCoords;
}vsOut;

uniform mat4 modelMatrix;

void main()
{
	vsOut.texCoords = tIn;
	//NOTE: no aplicaban la rotación de la vista!!!!
	mat3 nMatrix = mat3(transpose(inverse(view * modelMatrix)));
	vsOut.normal = normalize(nMatrix * nIn);
	gl_Position = projection * view * modelMatrix * vec4(pIn, 1.0);
}