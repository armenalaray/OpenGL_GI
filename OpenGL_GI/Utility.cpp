#include "Utility.h"

void SeedInit()
{
	srand((unsigned)time(NULL));
}

float randZeroToPlusOne()
{
	// NOTE(Alex): I want a number from 0 <= rand < 1 
	float Result = (float)rand() / (float)((RAND_MAX)+1);
	return Result;
}

float randMinOneToPlusOne()
{
	float Result = randZeroToPlusOne() * 2.0f - 1.0f;
	return Result;
}

glm::vec3 randInUnitDisc()
{
	glm::vec3 Result = glm::vec3(1.0, 0, 0);
	Result = ((2.0f * glm::vec3(randZeroToPlusOne(), randZeroToPlusOne(), 0.0f)) - glm::vec3(1.0f, 1.0f, 0.0f));
	Result = glm::normalize(Result);
	return Result;
}

glm::vec3 randomInUnitSphere()
{
	glm::vec3 Result(1.0, 0, 0);
	Result = ((2.0f * glm::vec3(randZeroToPlusOne(), randZeroToPlusOne(), randZeroToPlusOne())) - glm::vec3(1.0f, 1.0f, 1.0f));
	Result = glm::normalize(Result);
	return Result;
}


GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
