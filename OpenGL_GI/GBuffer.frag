#version 330 core

layout (location = 0) out vec3 gP;
layout (location = 1) out vec3 gN;
layout (location = 2) out vec4 gDiffSpec;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
	sampler2D emission;
	float shininess;
};

in VSBLOCK
{
	vec3 objectPos;
	vec3 normal;
	vec2 texCoords;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
}fsIn;

uniform Material mat;

void main()
{
	gP = fsIn.tangentFragPos;
	gN = fsIn.normal;
	gDiffSpec.rgb = texture(mat.texture_diffuse1, fsIn.texCoords).rgb;
	gDiffSpec.a = texture(mat.texture_specular1, fsIn.texCoords).r;
}