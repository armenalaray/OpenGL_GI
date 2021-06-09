#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath)
{
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	std::string geometryShaderSource;

	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;
	std::ifstream geometryShaderFile;

	//ensure ifstream objects can throw exceptions:
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	geometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		std::stringstream vertexShaderStream, fragmentShaderStream, geometryShaderStream;

		vertexShaderFile.open(vertexShaderPath);
		vertexShaderStream << vertexShaderFile.rdbuf();
		vertexShaderFile.close();
		vertexShaderSource = vertexShaderStream.str();

		fragmentShaderFile.open(fragmentShaderPath);
		fragmentShaderStream << fragmentShaderFile.rdbuf();
		fragmentShaderFile.close();
		fragmentShaderSource = fragmentShaderStream.str();

		if (geometryShaderPath)
		{
			geometryShaderFile.open(geometryShaderPath);
			geometryShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryShaderSource = geometryShaderStream.str();
		}

	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}


	//VERTEX SHADER
	unsigned int vertexShader;
	{
		const char* vss = vertexShaderSource.c_str();
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glCheckError();
		glShaderSource(vertexShader, 1, &vss, NULL);
		glCheckError();
		glCompileShader(vertexShader);
		glCheckError();

		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		glCheckError();
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR AT: " << vertexShaderPath << ", COMPILATION_FAILED.\n" << infoLog << std::endl;
		}
		else
		{
			std::cout << vertexShaderPath << " COMPILATION_SUCCESS\n";
		}
	}

	//FRAGMENT SHADER
	unsigned int fragmentShader;
	{
		const char* fss = fragmentShaderSource.c_str();

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glCheckError();
		glShaderSource(fragmentShader, 1, &fss, NULL);
		glCheckError();
		glCompileShader(fragmentShader);
		glCheckError();

		int success;
		char infoLog[512];
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		glCheckError();
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR AT: " << fragmentShaderPath << ", COMPILATION_FAILED.\n" << infoLog << std::endl;
		}
		else
		{
			std::cout << fragmentShaderPath << " COMPILATION_SUCCESS\n";
		}
	}

	//GEOMETRY SHADER
	unsigned int geometryShader;
	if (geometryShaderPath)
	{
		const char* gss = geometryShaderSource.c_str();

		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glCheckError();
		glShaderSource(geometryShader, 1, &gss, NULL);
		glCheckError();
		glCompileShader(geometryShader);
		glCheckError();

		int success;
		char infoLog[512];
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		glCheckError();
		if (!success)
		{
			glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
			std::cout << "ERROR AT: " << geometryShaderPath << ", COMPILATION_FAILED.\n" << infoLog << std::endl;
		}
		else
		{
			std::cout << geometryShaderPath << " COMPILATION_SUCCESS\n";
		}
	}

	//SHADER PROGRAM
	programID = glCreateProgram();
	glCheckError();
	glAttachShader(programID, vertexShader);
	glCheckError();
	if (geometryShaderPath)
	{
		glAttachShader(programID, geometryShader);
		glCheckError();
	}
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
	if (geometryShaderPath)
	{
		glDeleteShader(geometryShader);
		glCheckError();
	}
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

void Shader::setVar(const char* varName, unsigned int value)
{
	int location = glGetUniformLocation(programID, varName);
	glCheckError();
	glUniform1ui(location, value);
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
