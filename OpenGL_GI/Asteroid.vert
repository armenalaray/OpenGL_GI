#version 330 core

layout (location = 0) in vec3 pIn;
layout (location = 1) in vec3 nIn;
layout (location = 2) in vec2 tIn;

//NOTA: este viene de otro buffer!!!
layout (location = 3) in mat4 modelMatrix;
//layout (location = 4) in mat4 modelMatrix;
//layout (location = 5) in mat4 modelMatrix;
//layout (location = 6) in mat4 modelMatrix;

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

void main()
{
	vsOut.texCoords = tIn;
	//NOTE: no aplicaban la rotaci?n de la vista!!!!
	mat3 nMatrix = mat3(transpose(inverse(view * modelMatrix)));
	vsOut.normal = normalize(nMatrix * nIn);
	gl_Position = projection * view * modelMatrix * vec4(pIn, 1.0);
}