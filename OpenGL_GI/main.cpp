#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <map>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

/*
*
* YOUAREHER: Data Structures - https://assimp-docs.readthedocs.io/en/latest/usage/use_the_lib.html
*
*
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

define material properties specific to each surface!!!
*/

struct FrameBuffer
{
	unsigned int id;
	unsigned int texColorBufferID;
	unsigned int renderBuffDepthStencilBufferID;
};


int viewPortWidth = 800;
int viewPortHeight = 600;

float deltaTime = 0.0f;
float currentFrame = 0.0f;
float lastFrame = 0.0f;

bool fKeyPressed = false;
extern bool activeFocus = true;

Camera camera(viewPortWidth, viewPortHeight);

int specularExp = 1;

const unsigned int CUBE_POSITION_COUNT = 2;

glm::vec3 cubePositions[CUBE_POSITION_COUNT] = {
	glm::vec3(-1.0f,  0.0f,  -1.0f),
	glm::vec3(0.0f,  0.0f,  0.0f),
};

const unsigned int POINT_LIGHT_COUNT = 4;
glm::vec3 pointLightPositions[POINT_LIGHT_COUNT] = {
glm::vec3(0.7f,  0.2f,  2.0f),
glm::vec3(2.3f, -3.3f, -4.0f),
glm::vec3(-4.0f,  2.0f, -12.0f),
glm::vec3(0.0f,  0.0f, -3.0f)
};

//glm::vec3 lightPos = glm::vec3((float)cubeCountX * 0.5f, (float)cubeCountY *0.5f, 5.0f);
glm::vec3 lightDir = glm::vec3(-0.5f, -0.2f, -0.5f);
glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);

glm::vec3 ambientLightIntensity = glm::vec3(0.1f, 0.1f, 0.1f);
glm::vec3 diffuseLightIntensity = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 specularLightIntensity = glm::vec3(1.0f, 1.0f, 1.0f);

float c = 1.0f;
float l = 0.09f;
float q = 0.032f;;


FrameBuffer fb = {};

//void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
//{
//	// ignore non-significant error/warning codes
//	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
//
//	std::cout << "---------------" << std::endl;
//	std::cout << "Debug message (" << id << "): " << message << std::endl;
//
//	switch (source)
//	{
//	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
//	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
//	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
//	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
//	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
//	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
//	} std::cout << std::endl;
//
//	switch (type)
//	{
//	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
//	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
//	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
//	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
//	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
//	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
//	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
//	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
//	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
//	} std::cout << std::endl;
//
//	switch (severity)
//	{
//	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
//	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
//	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
//	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
//	} std::cout << std::endl;
//	std::cout << std::endl;
//}

void CreateFrameBuffer(int width, int height)
{
	//FrameBuffer creation!

	glGenFramebuffers(1, &fb.id);
	glCheckError();
	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
	glCheckError();

	glGenTextures(1, &fb.texColorBufferID);
	glCheckError();
	glBindTexture(GL_TEXTURE_2D, fb.texColorBufferID);
	glCheckError();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glCheckError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glCheckError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glCheckError();
	glBindTexture(GL_TEXTURE_2D, 0);
	glCheckError();

	glGenRenderbuffers(1, &fb.renderBuffDepthStencilBufferID);
	glCheckError();
	glBindRenderbuffer(GL_RENDERBUFFER, fb.renderBuffDepthStencilBufferID);
	glCheckError();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glCheckError();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glCheckError();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.texColorBufferID, 0);
	glCheckError();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb.renderBuffDepthStencilBufferID);
	glCheckError();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	else
		std::cout << "FRAMEBUFFER:: Framebuffer is complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCheckError();
}

void DeleteFrameBuffer()
{
	glDeleteBuffers(1, &fb.texColorBufferID);
	glCheckError();
	glDeleteBuffers(1, &fb.renderBuffDepthStencilBufferID);
	glCheckError();
	glDeleteFramebuffers(1, &fb.id);
	glCheckError();
}

extern float calcDeltaTime()
{
	currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return deltaTime;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	//DeleteFrameBuffer();
	//CreateFrameBuffer(width, height);
}

//NOTE: xpos and ypos are screen pixel coordinates. 800 width * 600 height.
void mouse_callback(GLFWwindow* window, double mouseXPD, double mouseYPD)
{
	camera.rotate(mouseXPD, mouseYPD);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	//specularExp += (int)yOffset;
	//if (specularExp < 1) specularExp = 1;
	//if (specularExp > 1200) specularExp = 1200;

	camera.scroll(xOffset, yOffset);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		fKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && fKeyPressed)
	{
		fKeyPressed = false;
		if (activeFocus)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		activeFocus = !activeFocus;
		camera.setFirstMouse(true);
	}

	//TODO: enable cursor if out window not current!!!
	camera.move(window);
}

void RenderOutlinedCube(Shader& objectShader, Shader& lightShader, Shader& lightOutlineShader, unsigned int lightVAO, Model& bpModel)
{
	glEnable(GL_DEPTH_TEST);
	glCheckError();
	glDepthFunc(GL_LESS);
	glCheckError();
	glEnable(GL_STENCIL_TEST);
	glCheckError();
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glCheckError();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glCheckError();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glCheckError();


	//NOTE(Alex): do not write stencil buffer when drawing sponza!
	glStencilMask(0x00);

	//lightPos.x = cosf(0.5f * (float)glfwGetTime()) * 2.0f;
	//lightPos.z = sinf(0.5f * (float)glfwGetTime()) * 2.0f;
	//lightPos.y = 2.0f;

	objectShader.use();
	objectShader.setVar("viewPos", camera.getCameraPos());
	objectShader.setVar("mat.shininess", 32.0f);

	objectShader.setVar("near", camera.getNearPlane());
	objectShader.setVar("far", camera.getFarPlane());

	////SpotLight:
	objectShader.setVar("sLight.position", camera.getCameraPos());
	objectShader.setVar("sLight.spotDirection", camera.getCameraFront());
	objectShader.setVar("sLight.innerCutoff", cosf(glm::radians(12.0f)));
	objectShader.setVar("sLight.outerCutoff", cosf(glm::radians(17.0f)));

	objectShader.setVar("sLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	objectShader.setVar("sLight.diffuse", glm::vec3(0.0f, 0.4f, 0.0f));
	objectShader.setVar("sLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	//NOTE: 50 units range point light
	objectShader.setVar("sLight.c", 1.0f);
	objectShader.setVar("sLight.l", 0.045f);
	objectShader.setVar("sLight.q", 0.0075f);

	//DirectionalLight
	objectShader.setVar("dLight.direction", lightDir);
	objectShader.setVar("dLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	objectShader.setVar("dLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	objectShader.setVar("dLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	//PointLight 0:
	objectShader.setVar("pLight[0].ambient", ambientLightIntensity);
	objectShader.setVar("pLight[0].diffuse", diffuseLightIntensity);
	objectShader.setVar("pLight[0].specular", specularLightIntensity);
	objectShader.setVar("pLight[0].position", pointLightPositions[0]);
	//NOTE: 50 units range point light
	objectShader.setVar("pLight[0].c", c);
	objectShader.setVar("pLight[0].l", l);
	objectShader.setVar("pLight[0].q", q);

	////PointLight 1:
	//objectShader.setVar("pLight[1].ambient", ambientLightIntensity);
	//objectShader.setVar("pLight[1].diffuse", diffuseLightIntensity);
	//objectShader.setVar("pLight[1].specular", specularLightIntensity);
	//objectShader.setVar("pLight[1].position", pointLightPositions[1]);
	////NOTE: 50 units range point light
	//objectShader.setVar("pLight[1].c", c);
	//objectShader.setVar("pLight[1].l", l);
	//objectShader.setVar("pLight[1].q", q);
	//
	////PointLight 2:
	//objectShader.setVar("pLight[2].ambient", ambientLightIntensity);
	//objectShader.setVar("pLight[2].diffuse", diffuseLightIntensity);
	//objectShader.setVar("pLight[2].specular", specularLightIntensity);
	//objectShader.setVar("pLight[2].position", pointLightPositions[2]);
	////NOTE: 50 units range point light
	//objectShader.setVar("pLight[2].c", c);
	//objectShader.setVar("pLight[2].l", l);
	//objectShader.setVar("pLight[2].q", q);
	//
	////PointLight 3:
	//objectShader.setVar("pLight[3].ambient", ambientLightIntensity);
	//objectShader.setVar("pLight[3].diffuse", diffuseLightIntensity);
	//objectShader.setVar("pLight[3].specular", specularLightIntensity);
	//objectShader.setVar("pLight[3].position", pointLightPositions[3]);
	////NOTE: 50 units range point light
	//objectShader.setVar("pLight[3].c", c);
	//objectShader.setVar("pLight[3].l", l);
	//objectShader.setVar("pLight[3].q", q);

			//objectShader.setVar("texture0", 0);
		//objectShader.setVar("texture1", 1);

		//glm::vec3 lightColor;
		//lightColor.r = sin(glfwGetTime() * 2.0f);
		//lightColor.g = sin(glfwGetTime() * 0.7f);
		//lightColor.b = sin(glfwGetTime() * 1.3f);
		//
		//glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		//glm::vec3 ambientColor = lightColor * glm::vec3(0.2f);
		//objectShader.setVar("light.ambient", ambientColor);
		//objectShader.setVar("light.diffuse", diffuseColor);


		//objectShader.setVar("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		//objectShader.setVar("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		//objectShader.setVar("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		////objectShader.setVar("light.position", lightPos);
		////NOTE: directional light!!!
		//objectShader.setVar("light.lightVector", lightPos);
		////NOTE: 50 units range point light
		//objectShader.setVar("light.c", 1.0f);
		//objectShader.setVar("light.l", 0.09f);
		//objectShader.setVar("light.q", 0.032f);


		////DirectionalLight
		//objectShader.setVar("dLight.direction", lightDir);
		//objectShader.setVar("dLight.ambient",  glm::vec3(0.1f, 0.1f, 0.1f));
		//objectShader.setVar("dLight.diffuse",  glm::vec3(0.4f, 0.0f, 0.0f));
		//objectShader.setVar("dLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));





		//glBindVertexArray(objectVAO);

		//for(int i = 0;i < 10; ++i)
		//{
		//	//glActiveTexture(GL_TEXTURE0);
		//	//glBindTexture(GL_TEXTURE_2D, texture0ID);
		//	//glActiveTexture(GL_TEXTURE1);
		//	//glBindTexture(GL_TEXTURE_2D, texture1ID);
		//	//glActiveTexture(GL_TEXTURE2);
		//	//glBindTexture(GL_TEXTURE_2D, texture2ID);

		//	objectShader.setVar("mat.diffuse", 0);
		//	objectShader.setVar("mat.specular", 1);
		//	objectShader.setVar("mat.emission", 2);
		//	objectShader.setVar("mat.shininess", 0.6f * 128.0f);

		//	glm::mat4 projection = camera.getProjectionMatrix();
		//	unsigned int perspLoc = glGetUniformLocation(objectShader.GetID(), "projection");
		//	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//	glm::mat4 view = camera.getViewMatrix();
		//	unsigned int viewLoc = glGetUniformLocation(objectShader.GetID(), "view");
		//	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, cubePositions[i]);
		//	//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		//	//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		//	unsigned int modelLoc = glGetUniformLocation(objectShader.GetID(), "model");
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//}

		////glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//
		//glBindVertexArray(0);

	glm::mat4 projection = camera.getProjectionMatrix();
	unsigned int perspLoc = glGetUniformLocation(objectShader.GetID(), "projection");
	glCheckError();
	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glCheckError();

	glm::mat4 view = camera.getViewMatrix();
	unsigned int viewLoc = glGetUniformLocation(objectShader.GetID(), "view");
	glCheckError();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glCheckError();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, 0, -5.0f));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
	unsigned int modelLoc = glGetUniformLocation(objectShader.GetID(), "model");
	glCheckError();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glCheckError();

	bpModel.Draw(objectShader);

	lightShader.use();

	//NOTE: first pass draw normal object!
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF); //enable writing to the stencil buffer
	glBindVertexArray(lightVAO);

	for (int i = 0; i < 1; ++i)
	{
		glm::mat4 projection = camera.getProjectionMatrix();
		unsigned int perspLoc = glGetUniformLocation(lightShader.GetID(), "projection");
		glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 view = camera.getViewMatrix();
		unsigned int viewLoc = glGetUniformLocation(lightShader.GetID(), "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 lightModel = glm::mat4(1.0f);
		//lightModel = glm::rotate(lightModel, glm::radians(0.1f * (float)glfwGetTime()), glm::vec3(0.0f, 0.0f, 0.0f));
		lightModel = glm::translate(lightModel, pointLightPositions[i]);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f, 0.2f, 0.2f));
		unsigned int modelLoc = glGetUniformLocation(lightShader.GetID(), "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lightModel));

		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);

	lightOutlineShader.use();

	//NOTE: first pass draw normal object!
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00); //enable writing to the stencil buffer
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(lightVAO);

	for (int i = 0; i < 1; ++i)
	{
		glm::mat4 projection = camera.getProjectionMatrix();
		unsigned int perspLoc = glGetUniformLocation(lightShader.GetID(), "projection");
		glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 view = camera.getViewMatrix();
		unsigned int viewLoc = glGetUniformLocation(lightShader.GetID(), "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 lightModel = glm::mat4(1.0f);
		//lightModel = glm::rotate(lightModel, glm::radians(0.1f * (float)glfwGetTime()), glm::vec3(0.0f, 0.0f, 0.0f));
		lightModel = glm::translate(lightModel, pointLightPositions[i]);
		lightModel = glm::scale(lightModel, glm::vec3(0.3f, 0.3f, 0.3f));
		unsigned int modelLoc = glGetUniformLocation(lightShader.GetID(), "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lightModel));

		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF); //enable writing to the stencil buffer
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		//NOTE: technically this is wrong, we cannot  pass GL_RGBA into format!!!
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		std::cout << "Texture Loaded at path: " << path << std::endl;
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


int main()
{
	calcDeltaTime();

	glfwInit();
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
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

	activeFocus = true;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);
	glViewport(0, 0, 800, 600);
	/*
	We register the callback functions after we've created the window and before the render loop is initiated.
	*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// configure global opengl state
	// -----------------------------
	/*glEnable(GL_DEPTH_TEST);
	glCheckError();
	glEnable(GL_BLEND);
	glCheckError();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCheckError();
	glEnable(GL_CULL_FACE);
	glCheckError();
	glCullFace(GL_BACK);
	glCheckError();*/

	//int flags;
	//glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	//if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	//{
	//	glEnable(GL_DEBUG_OUTPUT);
	//	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//	glDebugMessageCallback(glDebugOutput, nullptr);
	//	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	//}

	Shader objectShader("Object.vert", "Object.frag");
	//Shader lightShader("Light.vert", "Light.frag");
	//Shader lightOutlineShader("Light.vert", "OutlineObject.frag");

	Shader windowShader("Object.vert", "Window.frag");
	Shader floorShader("Object.vert", "Floor.frag");
	Shader frameBufferShader("FrameBufferQuad.vert", "FrameBufferQuad.frag");
	Shader skyBoxShader("CubeMap.vert", "CubeMap.frag");

	//Shader shader("Object.vert", "Window.frag");

	Model bpModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\backpack\\backpack.obj");
	//Model bpModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\sponza\\sponza.obj");

	float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
	};

	float planeVertices[] = {
		// positions          // texture Coords 
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	};
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int cubeVao;
	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);

	unsigned int cubeVbo;
	glGenBuffers(1, &cubeVbo);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	unsigned int planeVao;
	glGenVertexArrays(1, &planeVao);
	glBindVertexArray(planeVao);

	unsigned int planeVbo;
	glGenBuffers(1, &planeVbo);
	glBindBuffer(GL_ARRAY_BUFFER, planeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	unsigned int floorTexture = loadTexture("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Textures\\crane_diffuse.png");

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	unsigned int transparentVao;
	unsigned int transparentVbo;
	glGenVertexArrays(1, &transparentVao);
	glGenBuffers(1, &transparentVbo);

	glBindVertexArray(transparentVao);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);

	unsigned int transparentTexture = loadTexture("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Textures\\transparent_window.png");

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int FrameBufferQuadVao;
	glGenVertexArrays(1, &FrameBufferQuadVao);
	glBindVertexArray(FrameBufferQuadVao);
	unsigned int FrameBufferQuadVbo;
	glGenBuffers(1, &FrameBufferQuadVbo);
	glBindBuffer(GL_ARRAY_BUFFER, FrameBufferQuadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	//unsigned int transparentTexture = loadTexture("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Textures\\transparent_window.png");

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	CreateFrameBuffer(viewPortWidth, viewPortHeight);
	unsigned int frameBuffer = fb.id;
	unsigned int texColorBuffer = fb.texColorBufferID;

	//SKYBOX

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//NOTE: We don´t need texture coordinates for sky box since each vertex position is interoplated and that's the direction vector we need!!!
	unsigned int skyBoxVao;
	glGenVertexArrays(1, &skyBoxVao);
	glBindVertexArray(skyBoxVao);
	unsigned int skyBoxVbo;
	glGenBuffers(1, &skyBoxVbo);
	glBindBuffer(GL_ARRAY_BUFFER, skyBoxVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	std::vector<std::string> cubeMapPaths;
	cubeMapPaths.push_back("Source\\Textures\\skybox\\right.jpg");
	cubeMapPaths.push_back("Source\\Textures\\skybox\\left.jpg");
	cubeMapPaths.push_back("Source\\Textures\\skybox\\top.jpg");
	cubeMapPaths.push_back("Source\\Textures\\skybox\\bottom.jpg");
	cubeMapPaths.push_back("Source\\Textures\\skybox\\front.jpg");
	cubeMapPaths.push_back("Source\\Textures\\skybox\\back.jpg");

	unsigned int cubeMap;
	glGenTextures(1, &cubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	unsigned int i = 0;
	for (std::string path : cubeMapPaths)
	{
		int width, height, chaCount;
		unsigned char* imgData;
		imgData = stbi_load(path.c_str(), &width, &height, &chaCount, 0);
		if (imgData)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
			std::cout << "LOADING TEXTURE SUCCESS at path: " << path << std::endl;
			stbi_image_free(imgData);
		}
		else
		{
			std::cout << "ERROR::LOADING TEXTURE at path: " << path << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



	// transparent window locations
	// --------------------------------
	vector<glm::vec3> windows
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	//const int cubeCountX = 5;
	//const int cubeCountY = 4;
	//glm::vec3 cubePositions[cubeCountY][cubeCountX] = {};
	//for (int j = 0; j < cubeCountY; j++)
	//{
	//	for (int i = 0; i < cubeCountX; i++)
	//	{
	//		cubePositions[j][i] = glm::vec3(i, j, 0);
	//	}
	//}

	std::vector<glm::vec3> windowPs;
	windowPs.push_back(glm::vec3(-1.0f, 0.0f, -1.0f));
	windowPs.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	windowPs.push_back(glm::vec3(0.0f, 0.0f, -2.0f));
	windowPs.push_back(glm::vec3(0.0f, 0.0f, 10.0f));
	windowPs.push_back(glm::vec3(0.0f, 0.0f, -5.0f));
	windowPs.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
	windowPs.push_back(glm::vec3(0.0f, 0.0f, 0.0f));




	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCheckError();
		glEnable(GL_DEPTH_TEST);
		glCheckError();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glCheckError();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glCheckError();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCheckError();

		{
			objectShader.use();

			glm::mat4 projection = camera.getProjectionMatrix();
			unsigned int perspLoc = glGetUniformLocation(objectShader.GetID(), "projection");
			glCheckError();
			glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glCheckError();

			glm::mat4 view = camera.getViewMatrix();
			unsigned int viewLoc = glGetUniformLocation(objectShader.GetID(), "view");
			glCheckError();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glCheckError();

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0, 0, -5.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
			//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
			unsigned int modelLoc = glGetUniformLocation(objectShader.GetID(), "model");
			glCheckError();
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glCheckError();

			bpModel.Draw(objectShader);
		}

		{
			glDepthFunc(GL_LEQUAL);
			skyBoxShader.use();
			skyBoxShader.setVar("cubeMap", 0);
			glm::mat4 projection = camera.getProjectionMatrix();
			unsigned int perspLoc = glGetUniformLocation(skyBoxShader.GetID(), "projection");
			glCheckError();
			glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glCheckError();

			glm::mat4 view = camera.getViewMatrix();
			view = glm::mat4(glm::mat3(view));
			unsigned int viewLoc = glGetUniformLocation(skyBoxShader.GetID(), "view");
			glCheckError();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glCheckError();

			glBindVertexArray(skyBoxVao);
			glCheckError();
			glActiveTexture(GL_TEXTURE0);
			glCheckError();
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
			glCheckError();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glCheckError();
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glCheckError();
			glBindVertexArray(0);
			glCheckError();
			glDepthFunc(GL_LESS);
		}
		//2nd pass render FrameBufferQuad

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glCheckError();
		//glDisable(GL_DEPTH_TEST);
		//glCheckError();
		////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		////glCheckError();
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glCheckError();
		//glClear(GL_COLOR_BUFFER_BIT);
		//glCheckError();

		//frameBufferShader.use();
		//glBindVertexArray(FrameBufferQuadVao);
		//glCheckError();

		//frameBufferShader.setVar("screenTexture", 0);
		//glActiveTexture(GL_TEXTURE0);
		//glCheckError();
		//glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		//glCheckError();

		//glm::mat4 projection = camera.getProjectionMatrix();
		//unsigned int perspLoc = glGetUniformLocation(frameBufferShader.GetID(), "projection");
		//glCheckError();
		//glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));
		//glCheckError();

		//glm::mat4 view = camera.getViewMatrix();
		//unsigned int viewLoc = glGetUniformLocation(frameBufferShader.GetID(), "view");
		//glCheckError();
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		//glCheckError();

		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0, 0, -5.0f));
		//model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		////model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		//unsigned int modelLoc = glGetUniformLocation(frameBufferShader.GetID(), "model");
		//glCheckError();
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glCheckError();

		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glCheckError();
		//glBindVertexArray(0);
		//glCheckError();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	DeleteFrameBuffer();

	glfwTerminate();
	return 0;
}

//std::map<float, glm::vec3, std::greater<float>> sorted;
//for (unsigned int i = 0; i < windowPs.size(); i++)
//{
//	float dist = glm::length(windowPs[i] - camera.getCameraPos());
//	sorted[dist] = windowPs[i];
//}

//{
//	windowShader.use();
//	windowShader.setVar("t0", 0);
//	glBindVertexArray(transparentVao);
//	glCheckError();	
//	glActiveTexture(GL_TEXTURE0);
//	glCheckError();
//	glBindTexture(GL_TEXTURE_2D, transparentTexture);
//	glCheckError();
//	glm::mat4 projection = camera.getProjectionMatrix();
//	unsigned int perspLoc = glGetUniformLocation(windowShader.GetID(), "projection");
//	glCheckError();
//	glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(projection));
//	glCheckError();
//
//	glm::mat4 view = camera.getViewMatrix();
//	unsigned int viewLoc = glGetUniformLocation(windowShader.GetID(), "view");
//	glCheckError();
//	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//	glCheckError();
//
//	for(std::map<float,glm::vec3, std::greater<float>>::iterator it=sorted.begin(); it != sorted.end(); ++it)
//	{
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::translate(model, it->second);
//		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
//		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
//		unsigned int modelLoc = glGetUniformLocation(windowShader.GetID(), "model");
//		glCheckError();
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		glCheckError();
//
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		glCheckError();
//
//	}
//
//	
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glCheckError();
//
//	//glDisable(GL_BLEND);
//	//glCheckError();
//
//	glBindVertexArray(0);
//	glCheckError();
//
//}
