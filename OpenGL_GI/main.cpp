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
#include "Utility.h"

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
	viewPortHeight = height;
	viewPortWidth = width;
	glViewport(0, 0, viewPortWidth, viewPortHeight);
	camera.setViewPort(viewPortWidth, viewPortHeight);

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



int main()
{
	calcDeltaTime();

	glfwInit();
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

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

	glEnable(GL_MULTISAMPLE);

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

	//Shader objectNormal("ObjectNormal.vert", "ObjectNormal.frag");
	//Shader displayNormal("DisplayNormal.vert", "DisplayNormal.frag", "DisplayNormal.geom");
	//Shader explodeShader("Explode.vert", "Explode.frag", "Explode.geom");

	//Shader quadInstancing("QuadInstancing.vert", "QuadInstancing.frag");

	Shader planetShader("Planet.vert", "Planet.frag");
	Shader asteroidsShader("Asteroid.vert", "Asteroid.frag");

	stbi_set_flip_vertically_on_load(true);

	//Model bpModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\backpack\\backpack.obj");
	Model planetModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\planet\\planet.obj");
	Model rockModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\rock\\rock.obj");

	//Model bpModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\sponza\\sponza.obj");


	//float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	//	// positions   // texCoords
	//	-1.0f,  1.0f,  0.0f, 1.0f,
	//	-1.0f, -1.0f,  0.0f, 0.0f,
	//	 1.0f, -1.0f,  1.0f, 0.0f,
	//
	//	-1.0f,  1.0f,  0.0f, 1.0f,
	//	 1.0f, -1.0f,  1.0f, 0.0f,
	//	 1.0f,  1.0f,  1.0f, 1.0f
	//};
	//
	//unsigned int FrameBufferQuadVao;
	//glGenVertexArrays(1, &FrameBufferQuadVao);
	//glBindVertexArray(FrameBufferQuadVao);
	//unsigned int FrameBufferQuadVbo;
	//glGenBuffers(1, &FrameBufferQuadVbo);
	//glBindBuffer(GL_ARRAY_BUFFER, FrameBufferQuadVbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	//
	////unsigned int transparentTexture = loadTexture("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Textures\\transparent_window.png");
	//
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//
	//glBindVertexArray(0);
	//
	//CreateFrameBuffer(viewPortWidth, viewPortHeight);
	//unsigned int frameBuffer = fb.id;
	//unsigned int texColorBuffer = fb.texColorBufferID;
	//
	////SKYBOX
	//
	//float skyboxVertices[] = {
	//	// positions          
	//	-1.0f,  1.0f, -1.0f,
	//	-1.0f, -1.0f, -1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//	 1.0f,  1.0f, -1.0f,
	//	-1.0f,  1.0f, -1.0f,
	//
	//	-1.0f, -1.0f,  1.0f,
	//	-1.0f, -1.0f, -1.0f,
	//	-1.0f,  1.0f, -1.0f,
	//	-1.0f,  1.0f, -1.0f,
	//	-1.0f,  1.0f,  1.0f,
	//	-1.0f, -1.0f,  1.0f,
	//
	//	 1.0f, -1.0f, -1.0f,
	//	 1.0f, -1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f, -1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//
	//	-1.0f, -1.0f,  1.0f,
	//	-1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f, -1.0f,  1.0f,
	//	-1.0f, -1.0f,  1.0f,
	//
	//	-1.0f,  1.0f, -1.0f,
	//	 1.0f,  1.0f, -1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	-1.0f,  1.0f,  1.0f,
	//	-1.0f,  1.0f, -1.0f,
	//
	//	-1.0f, -1.0f, -1.0f,
	//	-1.0f, -1.0f,  1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//	-1.0f, -1.0f,  1.0f,
	//	 1.0f, -1.0f,  1.0f
	//};
	//
	////NOTE: We don´t need texture coordinates for sky box since each vertex position is interoplated and that's the direction vector we need!!!
	//unsigned int skyBoxVao;
	//glGenVertexArrays(1, &skyBoxVao);
	//glBindVertexArray(skyBoxVao);
	//unsigned int skyBoxVbo;
	//glGenBuffers(1, &skyBoxVbo);
	//glBindBuffer(GL_ARRAY_BUFFER, skyBoxVbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glBindVertexArray(0);
	//
	//std::vector<std::string> cubeMapPaths;
	//cubeMapPaths.push_back("Source\\Textures\\skybox\\right.jpg");
	//cubeMapPaths.push_back("Source\\Textures\\skybox\\left.jpg");
	//cubeMapPaths.push_back("Source\\Textures\\skybox\\top.jpg");
	//cubeMapPaths.push_back("Source\\Textures\\skybox\\bottom.jpg");
	//cubeMapPaths.push_back("Source\\Textures\\skybox\\front.jpg");
	//cubeMapPaths.push_back("Source\\Textures\\skybox\\back.jpg");
	//
	//unsigned int cubeMap;
	//glGenTextures(1, &cubeMap);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	//unsigned int i = 0;
	//for (std::string path : cubeMapPaths)
	//{
	//	int width, height, chaCount;
	//	unsigned char* imgData;
	//	imgData = stbi_load(path.c_str(), &width, &height, &chaCount, 0);
	//	if (imgData)
	//	{
	//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
	//		std::cout << "LOADING TEXTURE SUCCESS at path: " << path << std::endl;
	//		stbi_image_free(imgData);
	//	}
	//	else
	//	{
	//		std::cout << "ERROR::LOADING TEXTURE at path: " << path << std::endl;
	//	}
	//}
	//
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//
	//
	//// Test Geometry Shader
	//float points[] = {
	//-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
	// 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
	// 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
	//-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
	//};
	//
	//unsigned int houseVao;
	//glGenVertexArrays(1, &houseVao);
	//glBindVertexArray(houseVao);
	//
	//unsigned int houseVbo;
	//glGenBuffers(1, &houseVbo);
	//glBindBuffer(GL_ARRAY_BUFFER, houseVbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//
	//glBindVertexArray(0);

	float quadVertices[] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	glm::vec2 offsets[100];
	int index = 0;
	float baseOffset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec2 offset;
			offset.x = (float)x / 10.0f + baseOffset;
			offset.y = (float)y / 10.0f + baseOffset;
			offsets[index++] = offset;
		}
	}


	unsigned int quadVao;
	glGenVertexArrays(1, &quadVao);
	glBindVertexArray(quadVao);

	unsigned int quadVbo;
	glGenBuffers(1, &quadVbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	unsigned int offsetVbo;
	glGenBuffers(1, &offsetVbo);
	glBindBuffer(GL_ARRAY_BUFFER, offsetVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &offsets[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//NOTE: this is an instanced array!
	glBindBuffer(GL_ARRAY_BUFFER, offsetVbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	glBindVertexArray(0);

	//Asteriod model transform matrices!
	SeedInit();

	const unsigned int asteroidCount = 3000;
	glm::mat4* modelMatrices = new glm::mat4[asteroidCount];
	glm::vec3 center = glm::vec3(0, -10, -50);
	glm::vec3 dir = glm::vec3(1, 0, 0);
	glm::vec3 pivot = glm::vec3(0, 1, 0);

	float radius = 100.0f;
	float offset = 20.0f;
	float offsetHeight = 10.0f;
	for (unsigned int i = 0; i < asteroidCount; ++i)
	{
		float height = offsetHeight * randMinOneToPlusOne();
		float r = radius + offset * randMinOneToPlusOne();
		glm::vec3 p = r * dir + height * pivot;
		float angle = randZeroToPlusOne() * 360.0f;
		float asteroidAngle = randZeroToPlusOne() * 360.0f;

		float scaleWidth = randZeroToPlusOne() * 1.0f;
		
		glm::vec3 scaleVec = glm::vec3(scaleWidth, scaleWidth, scaleWidth);
		glm::vec3 asteroidPivot = randomInUnitSphere();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, center);
		model = glm::rotate(model, glm::radians(angle), pivot);
		model = glm::translate(model, p);
		model = glm::scale(model, scaleVec);
		model = glm::rotate(model, glm::radians(asteroidAngle), asteroidPivot);
		modelMatrices[i] = model;
	}

	unsigned int asteroidModelMatricesVbo;
	glGenBuffers(1, &asteroidModelMatricesVbo);
	glBindBuffer(GL_ARRAY_BUFFER, asteroidModelMatricesVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * asteroidCount, &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < rockModel.meshes.size(); ++i)
	{
		unsigned int VAO = rockModel.meshes[i].VAO;
		glBindVertexArray(VAO);
		//El maximo que puedes poner es 4 flotantes
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribDivisor(3, 1);

		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(4);
		glVertexAttribDivisor(4, 1);

		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribDivisor(5, 1);

		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//NOTE: Uniform buffer objects!!!!!!!!!!!!!!

	unsigned int bidnMat = glGetUniformBlockIndex(asteroidsShader.ID, "Matrices");
	glCheckError();
	glUniformBlockBinding(asteroidsShader.ID, bidnMat, 0);
	glCheckError();

	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glCheckError();
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glCheckError();
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glCheckError();
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
	glCheckError();
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glCheckError();

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glEnable(GL_DEPTH_TEST);
		glCheckError();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glCheckError();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glCheckError();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCheckError();

		glm::mat4 projection = camera.getProjectionMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glCheckError();
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glCheckError();
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glCheckError();


		glm::mat4 view = camera.getViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glCheckError();
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glCheckError();
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glCheckError();

		planetShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, center);
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		planetShader.setMat4("modelMatrix", model);
		planetModel.Draw(planetShader);

		asteroidsShader.use();
		asteroidsShader.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rockModel.textures_loaded[0].id);
		for (unsigned int i = 0; i < rockModel.meshes.size(); i++)
		{
			glBindVertexArray(rockModel.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, (GLsizei) rockModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, asteroidCount);
			glBindVertexArray(0);
		}



		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0, 0.0, -0.5f));

		//{
		//	objectNormal.use();
		//	unsigned int modelLoc = glGetUniformLocation(objectNormal.GetID(), "model");
		//	glCheckError();
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	glCheckError();
		//
		//	bpModel.Draw(objectNormal);
		//}
		//
		//{
		//	displayNormal.use();
		//	unsigned int modelLoc = glGetUniformLocation(displayNormal.GetID(), "model");
		//	glCheckError();
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	glCheckError();
		//
		//	bpModel.Draw(displayNormal);
		//}
		//explodeShader.setVar("time", (float)glfwGetTime());


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//DeleteFrameBuffer();

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
