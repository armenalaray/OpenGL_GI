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
	glShaderSource(vertexShader, 1, &vss, NULL);
	glCompileShader(vertexShader);

	int verSuccess;
	char verInfoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &verSuccess);
	if (!verSuccess)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, verInfoLog);
		std::cout << "ERROR::VERTEX_SHADER_OBJECT::COMPILATION_FAILED\n" << verInfoLog << std::endl;
	}
	else
	{
		std::cout << "VERTEX_SHADER_OBJECT::COMPILATION_SUCCESS\n";
	}


	//FRAGMENT SHADER

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fss, NULL);
	glCompileShader(fragmentShader);

	int fragSuccess;
	char fragInfoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragSuccess);
	if (!fragSuccess)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, fragInfoLog);
		std::cout << "ERROR::FRAGMENT_SHADER_OBJECT::COMPILATION_FAILED\n" << fragInfoLog << std::endl;
	}
	else
	{
		std::cout << "FRAGMENT_SHADER_OBJECT::COMPILATION_SUCCESS\n";
	}


	//SHADER PROGRAM
	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	int programIDSuccess;
	char programIDInfoLog[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &programIDSuccess);

	if (!programIDSuccess)
	{
		glGetProgramInfoLog(programID, 512, NULL, programIDInfoLog);
		std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << programIDInfoLog << std::endl;
	}
	else
	{
		std::cout << "SHADER_PROGRAM::LINKING_SUCCESS\n";
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Shader::use()
{
	glUseProgram(programID);
}

void Shader::setVar(const char* varName, bool value)
{
	int location = glGetUniformLocation(programID, varName);
	glUniform1i(location, value);
}

void Shader::setVar(const char* varName, int value)
{
	int location = glGetUniformLocation(programID, varName);
	glUniform1i(location, value);
}

void Shader::setVar(const char* varName, float value)
{
	int location = glGetUniformLocation(programID, varName);
	glUniform1f(location, value);
}

void Shader::setVar(const char* varName, glm::vec3 v0)
{
	int location = glGetUniformLocation(programID, varName);
	glUniform3f(location, v0.x, v0.y, v0.z);
}

void Shader::setVar(const char* varName, glm::vec4 v0)
{
	int location = glGetUniformLocation(programID, varName);
	glUniform4f(location, v0.x, v0.y, v0.z, v0.w);
}

unsigned int Shader::GetID()
{
	return programID;
}
