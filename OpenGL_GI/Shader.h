#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Shader
{
public:
	//Reads Shaders files and Constructs shader program
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void Use();
	void SetVar(const char* varName, bool value);
	void SetVar(const char* varName, int value);
	void SetVar(const char* varName, float value);
	void SetVar(const char* varName, float v0, float v1, float v2, float v3);
	unsigned int  GetID();
private:
	unsigned int programID;
};

