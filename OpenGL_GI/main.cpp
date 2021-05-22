#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>


/*
GLSL useful features specifically targeted at vector and matrix manipulation.
vertex shader -> input is called vertex attribute
So each vertex attribute is a member of the data structure you define to use!!! in vertex shader.

rgba for colors
stpq for texture coordinates.

shader data types:
-in
-out
-uniform

index
so we recieve a vertex per vertex shader program thread!!!!
*/

/*
So you could generate more VAO's as needed in one call!!

*/


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	//const char* vertexShaderSource = "#version 330 core\nlayout (location=0) in vec3 aPos;\nvoid main(){\ngl_Position=vec4(aPos.x,aPos.y,aPos.z,1.0);}\n\0";
	//const char* fragmentShaderSource = "#version 330 core\nout vec4 FragColor;\nvoid main(){\nFragColor=vec4(1.0f,0.5f,0.2f,1.0f);}\n\0";

	const char* vertexShaderSource = "#version 330 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aColor;\nout vec3 ourColor;\nvoid main(){\ngl_Position=vec4(aPos,1.0);\nourColor=aColor;}\n\0";
	const char* fragmentShaderSource = "#version 330 core\nout vec4 FragColor;\nin vec3 ourColor;\nvoid main(){\nFragColor=vec4(ourColor,1.0);}\n\0";

	//const char* vertexShaderSource = "#version 330 core\nlayout (location=0) in vec3 aPos;\nvoid main(){\ngl_Position=vec4(aPos.x,aPos.y,aPos.z,1.0);}\n\0";
	//const char* fragmentShaderSource = "#version 330 core\nout vec4 FragColor;\nuniform vec4 ourColor;\nvoid main(){\nFragColor=ourColor;}\n\0";

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Mac OSX needs this!!!!!!!!
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL_GI", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	/*
	We register the callback functions after we've created the window and before the render loop is initiated.
	*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	//VERTEX SHADER

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
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
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
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

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int shaderProgramSuccess;
	char shaderProgramInfoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderProgramSuccess);

	if (!shaderProgramSuccess)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, shaderProgramInfoLog);
		std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << shaderProgramInfoLog << std::endl;
	}
	else
	{
		std::cout << "SHADER_PROGRAM::LINKING_SUCCESS\n";
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Object Generation

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	//Initialization code(done once(unless your object frequently changes))
	//1. bind Vertex Array Object
	glBindVertexArray(VAO);

	/*
	float vertices[] =
	{
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};
	*/
	
	float vertices[] = {
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	unsigned int indices[] = {
		0,1,3,
		1,2,3,
	};


	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	/*
	Each vertex attribute takes its data from memory managed by a VBO and which VBO it takes its data from (you can have multiple VBOs) is determined by the VBO
	currently bound to GL_ARRAY_BUFFER when calling glVertexAttribPointer. Since the previously defined VBO is still bound before calling glVertexAttribPointer
	vertex attribute 0 is now associated with its vertex data.

	index: specifies the index of the generic vertex attribute to be modified.
	size: specifies the number of components per generic vertex attribute. Must be 1,2,3,4

	stride: specifies the byte offset between consecutive generic vertex attributes.
	If stride is 0, the generic vertex attributes are understood to be tightly packed in the array.

	Specifies a offset of the first component of the first generic vertex attribute in the array
	int the data store of the buffer currently bound to the GL_ARRAY_BUFFER target.
	*/
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(0.3f, 0.2f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		//float timeValue = glfwGetTime();
		//float greenValue = (sinf(timeValue) + 1.0f) * 0.5f;
		//printf("%f\r",greenValue);
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}