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

We do not have to re send the data which is quite slow!!!
All  happens in the side of the GPU.

*/

/*
So you could generate more VAO's as needed in one call!!

OPenGL

OpenGL should have at least 16 texture units for you to use which you can activate using GL_TEXTURE0 to GL_TEXTURE15. 
We can use GL_TEXTURE0 + 8 for example.

*/

int viewPortWidth = 800;
int viewPortHeight = 600;

////Gram-Schmidt process
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0, 0, 0);
glm::vec3 up = glm::vec3(0, 1.f, 0);

glm::vec3 camZ = glm::normalize(cameraPos - cameraTarget);
glm::vec3 camX = glm::normalize(glm::cross(up, camZ));
glm::vec3 camY = glm::cross(camZ, camX);

glm::vec3 cameraFront = -camZ;
glm::vec3 cameraRight = camX;

float deltaTime = 0.0f;
float currentFrame = 0.0f;
float lastFrame = 0.0f;
float unitsToAdvance = 0.0f;
float cameraSpeed = 12.5f;

float lastMouseXP = ((float)viewPortWidth * 0.5f);
float lastMouseYP = ((float)viewPortHeight * 0.5f);

float yaw = -90.0f;
float pitch = 0.0f;

float calcDeltaTime()
{
	currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return deltaTime;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//NOTE: xpos and ypos are screen pixel coordinates. 800 width * 600 height.
void mouse_callback(GLFWwindow* window, double mouseXPD, double mouseYPD)
{
	float mouseXP = (float)mouseXPD;
	float mouseYP = (float)mouseYPD;

	float xOffset = mouseXP - lastMouseXP;
	float yOffset = lastMouseYP - mouseYP;
	lastMouseXP = mouseXP;
	lastMouseYP = mouseYP;

	const float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += unitsToAdvance * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos += -(unitsToAdvance * cameraRight);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos += -(unitsToAdvance * cameraFront);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += unitsToAdvance * cameraRight;

}

int main()
{
	calcDeltaTime();

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
	GLFWwindow* window = glfwCreateWindow(viewPortWidth, viewPortHeight, "OpenGL_GI", NULL, NULL);
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, 800, 600);
	/*
	We register the callback functions after we've created the window and before the render loop is initiated.
	*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

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
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	//float vertices[] = {
	//	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f,   //top-right
	//	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, -1.0f,  //bottom-right
	//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, //bottom-left
	//	-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 2.0f   //top-left
	//};
	//
	//unsigned int indices[] = {
	//	0,1,3,
	//	1,2,3,
	//};


	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	////vertex color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(1);
	
	//texture coordinates attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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
	
	glEnable(GL_DEPTH_TEST);

	ps.Use();
	ps.SetVar("texture0", 0);
	ps.SetVar("texture1", 1);

	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	
	
	while (!glfwWindowShouldClose(window))
	{
		unitsToAdvance = cameraSpeed * calcDeltaTime();
		process_input(window);

		glClearColor(0.3f, 0.2f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0ID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1ID);

		glBindVertexArray(VAO);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), (float)viewPortWidth / (float)viewPortHeight, 0.1f, 100.0f);
		unsigned int perspLoc = glGetUniformLocation(ps.GetID(), "projection");
		glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//float radius = 10.0f;
		//float camPosX = cosf((float)glfwGetTime()) * radius;
		//float camPosZ = sinf((float)glfwGetTime()) * radius;

		glm::vec3 dir;
		dir.x = cosf(glm::radians(pitch)) * cosf(glm::radians(yaw));
		dir.y = sinf(glm::radians(pitch));
		dir.z = cosf(glm::radians(pitch)) * sinf(glm::radians(yaw));
		cameraFront = glm::normalize(dir);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
		unsigned int viewLoc = glGetUniformLocation(ps.GetID(), "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


		for (unsigned int i = 0; i < 10; ++i) {
			glm::mat4 model = glm::mat4(1.0f);	
			//model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

			unsigned int modelLoc = glGetUniformLocation(ps.GetID(), "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}