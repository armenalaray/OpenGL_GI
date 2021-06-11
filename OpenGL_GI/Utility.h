#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLenum glCheckError_(const char* file, int line);
#ifdef _DEBUG
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
#else
#define glCheckError()
#endif

void SeedInit();
float randZeroToPlusOne();
float randMinOneToPlusOne();
glm::vec3 randInUnitDisc();
glm::vec3 randomInUnitSphere();
