#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
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

por eso es que se escala desde el vertex shader y no del fragment!!!
Para que la interpolación sea correcta!!!
*/

/*
So you could generate more VAO's as needed in one call!!

OPenGL

OpenGL should have at least 16 texture units for you to use which you can activate using GL_TEXTURE0 to GL_TEXTURE15. 
We can use GL_TEXTURE0 + 8 for example.

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

	//glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	////NOTE: initializaes identity matrix 4
	//glm::mat4 trans = glm::mat4(1.0f);
	//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	//vec = trans * vec;
	//std::cout << vec.x << " " << vec.y << " " << vec.z << " " << vec.w << " " << std::endl;

	//glm::mat4 trans = glm::mat4(1.0f);
	//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0, 0, 1));
	//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

	//const char* vertexShaderSource = "#version 330 core\nlayout (location=0) in vec3 aPos;\nvoid main(){\ngl_Position=vec4(aPos.x,aPos.y,aPos.z,1.0);}\n\0";
	//const char* fragmentShaderSource = "#version 330 core\nout vec4 FragColor;\nvoid main(){\nFragColor=vec4(1.0f,0.5f,0.2f,1.0f);}\n\0";

	//const char* vertexShaderSource = "#version 330 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aColor;\nout vec3 ourColor;\nvoid main(){\ngl_Position=vec4(aPos,1.0);\nourColor=aColor;}\n\0";
	//const char* fragmentShaderSource = "#version 330 core\nout vec4 FragColor;\nin vec3 ourColor;\nvoid main(){\nFragColor=vec4(ourColor,1.0);}\n\0";

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

	Shader ps("shader.vert", "shader.frag");

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
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f,   //top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, -1.0f,  //bottom-right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, //bottom-left
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 2.0f   //top-left
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

	//vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//vertex color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture coordinates attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Texture object generation

	unsigned int texture0ID;
	glGenTextures(1, &texture0ID);
	glBindTexture(GL_TEXTURE_2D, texture0ID);

	//set the texture wrapping/filtering options (on the currently bound texxture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, chaCount;
		//TODO: Check how we are receiving the data!!!
		unsigned char* data = stbi_load("Source\\Textures\\pelican.jpg", &width, &height, &chaCount, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "STB_IMAGE::LOADING_TEXTURE_SUCCESS" << std::endl;
		}
		else
		{
			std::cout << "ERROR::STB_IMAGE::FAILED TO LOAD TEXTURE" << std::endl;
		}
		stbi_image_free(data);
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);

	//Texture object generation

	unsigned int texture1ID;
	glGenTextures(1, &texture1ID);
	glBindTexture(GL_TEXTURE_2D, texture1ID);

	//set the texture wrapping/filtering options (on the currently bound texxture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, chaCount;
		//TODO: Check how we are receiving the data!!!
		unsigned char* data = stbi_load("Source\\Textures\\face.png", &width, &height, &chaCount, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "STB_IMAGE::LOADING_TEXTURE_SUCCESS" << std::endl;
		}
		else
		{
			std::cout << "ERROR::STB_IMAGE::FAILED TO LOAD TEXTURE" << std::endl;
		}
		stbi_image_free(data);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	
	ps.Use();
	ps.SetVar("texture0", 0);
	ps.SetVar("texture1", 1);

	
	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(0.3f, 0.2f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0, 0, 1));
		trans = glm::translate(trans, glm::vec3(0.5, -0.5, 0.0));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0, 0, 1));

		unsigned int transformLoc = glGetUniformLocation(ps.GetID(), "trans");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));


		//float timeValue = (float)glfwGetTime();
		//float greenValue = (sinf(timeValue) + 1.0f) * 0.5f;
		//printf("%f\r",greenValue);
		//ps.SetVar("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0ID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1ID);
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}