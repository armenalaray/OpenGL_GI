#version 330 core
out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

vec4 inverseColor()
{
	vec4 inverse = 1.0 - texture(screenTexture, texCoords);
	return vec4(vec3(inverse),1.0);
}

const float offset = 1.0 / 300.0;

void main()
{

	vec2 offsetBuffer[9] = vec2[]
	(
		vec2(-offset, offset),
		vec2(0.0f, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(offset, 0.0f),
		vec2(-offset, -offset),
		vec2(0.0f, -offset),
		vec2(offset, -offset)
	);

	float sharpKernel[9] = float[]
	(
		-1,-1,-1,
		-1, 9,-1,
		-1,-1,-1
	);

	float blurKernel[9] = float[]
	(
		1.0,2.0,1.0,
		2.0,4.0,2.0,
		1.0,2.0,1.0
	);

	float edgeDetectionKernel[9] = float[]
	(
		1.0,1.0,1.0,
		1.0,-8.0,1.0,
		1.0,1.0,1.0
	);

	vec3 sampleTex[9];
	for(int i=0; i < 9;++i)
	{
		sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offsetBuffer[i]));
	}

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; ++i)
	{
		col += sampleTex[i] * edgeDetectionKernel[i];
	}

	FragColor = vec4(col, 1.0);

	vec3 texColor  = texture(screenTexture, texCoords).rgb;
	FragColor = vec4(texColor,1.0);

	//vec4 texColor = texture(screenTexture, texCoords);
	//float avg  = (texColor.r + texColor.g + texColor.b) / 3.0;
	//FragColor = vec4(avg,avg,avg,1.0);

	//vec4 texColor = texture(screenTexture, texCoords);
	//float avg  = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
	//FragColor = vec4(avg,avg,avg,1.0);


	//FragColor = vec4(0.0,1.0,0.0,1.0);
}