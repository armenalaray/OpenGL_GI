#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

//layout (location = 2) in vec3 aNormal;
//layout (location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//out vec3 ourColor;
out vec2 texCoord;

//out vec3 normal;

out vec3 objectPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos,1.0);
	//gl_Position = vec4(aPos,1.0f);
	//this is wrong we need to pass the normal in world space not in object space.
	//NORMALS CORRECT!!
	//normal = transpose(inverse(mat3(model))) * aNormal;
	//normal = mat3(transpose(inverse(model))) * aNormal;
	objectPos = vec3(model * vec4(aPos,1.0));

	//ourColor = aColor;
	texCoord = aTexCoord;
}