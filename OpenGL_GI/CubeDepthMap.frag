#version 330 core
/*
Y aqui el fragment shader toma cada fragment de cada 
emito cara 0 con todos los vertices vistos desde cara 0

se hace el clipping respectivo, entonces solo los fragmentos visibles desde el frustrum de face 0 corren este programa

NOTE: how to add more precission into depth map!!!
*/
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
	float lightDistance = length(FragPos.xyz - lightPos);
	lightDistance = lightDistance / far_plane;
	gl_FragDepth = lightDistance;
}