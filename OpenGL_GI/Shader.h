#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Utility.h"

class Shader
{
public:
	//Reads Shaders files and Constructs shader program
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath = 0);
	void use();
	void setVar(const char* varName, bool value);
	void setVar(const char* varName, int value);
	void setVar(const char* varName, unsigned int value);
	void setVar(const char* varName, float value);
	void setVar(const char* varName, glm::vec3 v0);
	void setVar(const char* varName, glm::vec4 v0);
	unsigned int  GetID();
private:
	unsigned int programID;
};

