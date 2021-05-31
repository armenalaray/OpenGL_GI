#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

GLenum glCheckError_(const char* file, int line);
#ifdef _DEBUG
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
#else
#define glCheckError()
#endif