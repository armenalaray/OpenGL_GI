#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	std::string vertexShaderSource;
	std::string fragmentShaderSource;

	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;
	//ensure ifstream objects can throw exceptions:
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vertexShaderFile.open(vertexShaderPath);
		fragmentShaderFile.open(fragmentShaderPath);
		std::stringstream vertexShaderStream, fragmentShaderStream;

		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexShaderSource = vertexShaderStream.str();
		fragmentShaderSource = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vss = vertexShaderSource.c_str();
	const char* fss = fragmentShaderSource.c_str();

	//VERTEX SHADER

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glCheckError();
	glShaderSource(vertexShader, 1, &vss, NULL);
	glCheckError();
	glCompileShader(vertexShader);
	glCheckError();

	int verSuccess;
	char verInfoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &verSuccess);
	glCheckError();
	if (!verSuccess)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, verInfoLog);
		std::cout << "ERROR AT: " << vertexShaderPath << ", COMPILATION_FAILED.\n" << verInfoLog << std::endl;
	}
	else
	{
		std::cout << vertexShaderPath << " COMPILATION_SUCCESS\n";
	}


	//FRAGMENT SHADER

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glCheckError();
	glShaderSource(fragmentShader, 1, &fss, NULL);
	glCheckError();
	glCompileShader(fragmentShader);
	glCheckError();

	int fragSuccess;
	char fragInfoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragSuccess);
	glCheckError();
	if (!fragSuccess)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, fragInfoLog);
		std::cout << "ERROR AT: " << fragmentShaderPath << ", COMPILATION_FAILED.\n" << fragInfoLog << std::endl;
	}
	else
	{
		std::cout << fragmentShaderPath << " COMPILATION_SUCCESS\n";
	}


	//SHADER PROGRAM
	programID = glCreateProgram();
	glCheckError();
	glAttachShader(programID, vertexShader);
	glCheckError();
	glAttachShader(programID, fragmentShader);
	glCheckError();
	glLinkProgram(programID);
	glCheckError();

	int programIDSuccess;
	char programIDInfoLog[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &programIDSuccess);
	glCheckError();

	if (!programIDSuccess)
	{
		glGetProgramInfoLog(programID, 512, NULL, programIDInfoLog);
		std::cout << "ERROR: SHADER_PROGRAM LINKING_FAILED\n" << programIDInfoLog << std::endl;
	}
	else
	{
		std::cout << "SHADER_PROGRAM LINKING_SUCCESS\n";
	}

	glDeleteShader(vertexShader);
	glCheckError();
	glDeleteShader(fragmentShader);
	glCheckError();

}

void Shader::use()
{
	glUseProgram(programID);
	glCheckError();
}

void Shader::setVar(const char* varName, bool value)
{
	int location = glGetUniformLocation(programID, varName);
	glCheckError();
	glUniform1i(location, value);
	glCheckError();
}

void Shader::setVar(const char* varName, int value)
{
	int location = glGetUniformLocation(programID, varName);
	glCheckError();
	glUniform1i(location, value);
	glCheckError();
}

void Shader::setVar(const char* varName, float value)
{
	int location = glGetUniformLocation(programID, varName);
	glCheckError();
	glUniform1f(location, value);
	glCheckError();
}

void Shader::setVar(const char* varName, glm::vec3 v0)
{
	int location = glGetUniformLocation(programID, varName);
	glCheckError();
	glUniform3f(location, v0.x, v0.y, v0.z);
	glCheckError();
}

void Shader::setVar(const char* varName, glm::vec4 v0)
{
	int location = glGetUniformLocation(programID, varName);
	glCheckError();
	glUniform4f(location, v0.x, v0.y, v0.z, v0.w);
	glCheckError();
}

unsigned int Shader::GetID()
{
	return programID;
}
