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
#include "ShaderList.h"

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

enum gbuffer_tex_index : unsigned int
{
	GBUFFER_TEX_INDEX_POSITON,
	GBUFFER_TEX_INDEX_NORMAL,
	GBUFFER_TEX_INDEX_COLORSPEC,
};

struct Buffer2D
{
	unsigned int width;
	unsigned int height;
	unsigned int id;
};

struct FrameBuffer
{
	unsigned int id;
	std::unique_ptr<Buffer2D[]> colorBuffers;
	std::unique_ptr<Buffer2D[]> depthBuffers;
};


//float aspectRatio = 16.0f / 9.0f;
int viewPortHeight = 600;
int viewPortWidth = 800;

float deltaTime = 0.0f;
float currentFrame = 0.0f;
float lastFrame = 0.0f;

extern bool activeFocus = true;
bool fKeyPressed = false;

bool hdr = true;
bool hdrKeyPressed = false;
float exposure = 1.0f;

Camera camera(viewPortWidth, viewPortHeight);

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
	camera.setViewPort(viewPortWidth, viewPortHeight);
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

	float scale = 0.1f;
	exposure += (float)yOffset * scale;
	if (exposure < 0) exposure = 0;
	if (exposure > 10) exposure = 10;

	//camera.scroll(xOffset, yOffset);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !hdrKeyPressed)
	{
		hdr = !hdr;
		hdrKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		hdrKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fKeyPressed)
	{
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
		fKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		fKeyPressed = false;
	}

	//TODO: enable cursor if out window not current!!!
	camera.move(window);
}

void DrawFloor(Shader shader, unsigned int vao, unsigned int tid, glm::mat4 model)
{
	shader.use();
	shader.setInt("viewPortHalfWidth", (int)((float)viewPortWidth * 0.5f));
	shader.setVec3("viewPos", camera.getCameraPos());
	shader.setVec3("dLight.direction", glm::vec3(-0.5f, -0.5f, -0.5f));
	shader.setVec3("dLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("dLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVec3("dLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	shader.setInt("mat.texture_diffuse1", 0);
	shader.setFloat("mat.shininess", 64.0f);
	shader.setFloat("specularColor", 1.0f);

	shader.setMat4("modelMatrix", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tid);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
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

	GLFWmonitor* primary = glfwGetPrimaryMonitor();

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

	//glEnable(GL_CULL_FACE);
	//glCheckError();
	//glCullFace(GL_BACK);
	//glCheckError();

	//NOTE: this will apply gama correction for each framebuffer used, not the last framebuffer only!
	//glEnable(GL_FRAMEBUFFER_SRGB);
	/*glEnable(GL_DEPTH_TEST);
	glCheckError();
	glEnable(GL_BLEND);
	glCheckError();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCheckError();
	*/

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

	//Shader asteroidsShader("Asteroid.vert", "Asteroid.frag");
	//Shader phongShader("Phong.vert", "Phong.frag");
	//Shader blinnPhongShader("BlinnPhong.vert", "BlinnPhong.frag");

	ShaderList list;
	list.shaders.push_back(Shader("GBuffer.vert", "GBuffer.frag"));
	list.shaders.push_back(Shader("DeferredShading.vert", "DeferredShading.frag"));

	//list.shaders.push_back(Shader("Parallax.vert", "Parallax.frag"));
	//list.shaders.push_back(Shader("DebugLightPos.vert", "DebugLightPos.frag"));
	//list.shaders.push_back(Shader("HDR.vert", "HDR.frag"));
	//list.shaders.push_back(Shader("GaussianBlur.vert", "GaussianBlur.frag"));
	
	//list.shaders.push_back(Shader("Phong.vert", "Phong.frag"));
	//list.shaders.push_back(Shader("Lighting.vert", "Lighting.frag"));
	//list.shaders.push_back(Shader("LightDepth.vert", "LightDepth.frag"));
	//list.shaders.push_back(Shader("DepthBufferQuad.vert", "DepthBufferQuad.frag"));
	//list.shaders.push_back(Shader("CubeDepthMap.vert", "CubeDepthMap.frag", "CubeDepthMap.geom"));
	//list.shaders.push_back(Shader("DisplayNormal.vert", "DisplayNormal.frag", "DisplayNormal.geom"));

	if (list.ShadersAreValid())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		stbi_set_flip_vertically_on_load(true);

		//Model bpModel("Source\\Models\\cube\\cube.obj");
		//Model bpModel("Source\\Models\\\CornellBox\\CornellBox-Empty-RG.obj");

		Model bpModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\backpack\\backpack.obj");
		//Model planetModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\planet\\planet.obj");
		//Model rockModel("C:\\Source\\OpenGL_GI\\OpenGL_GI\\Source\\Models\\rock\\rock.obj");

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




		//float quadVertices[] = {
		//	// positions     // colors
		//	-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		//	 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		//	-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,
		//
		//	-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		//	 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		//	 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
		//};
		//
		//glm::vec2 offsets[100];
		//int index = 0;
		//float baseOffset = 0.1f;
		//for (int y = -10; y < 10; y += 2)
		//{
		//	for (int x = -10; x < 10; x += 2)
		//	{
		//		glm::vec2 offset;
		//		offset.x = (float)x / 10.0f + baseOffset;
		//		offset.y = (float)y / 10.0f + baseOffset;
		//		offsets[index++] = offset;
		//	}
		//}
		//
		//
		//unsigned int quadVao;
		//glGenVertexArrays(1, &quadVao);
		//glBindVertexArray(quadVao);
		//
		//unsigned int quadVbo;
		//glGenBuffers(1, &quadVbo);
		//glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		//
		//unsigned int offsetVbo;
		//glGenBuffers(1, &offsetVbo);
		//glBindBuffer(GL_ARRAY_BUFFER, offsetVbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &offsets[0], GL_STATIC_DRAW);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
		//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(0);
		//
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		//glEnableVertexAttribArray(1);
		//
		////NOTE: this is an instanced array!
		//glBindBuffer(GL_ARRAY_BUFFER, offsetVbo);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(2);
		//glVertexAttribDivisor(2, 1);
		//
		//glBindVertexArray(0);
		//
		////Asteriod model transform matrices!
		//SeedInit();
		//
		//const unsigned int asteroidCount = 3000;
		//glm::mat4* modelMatrices = new glm::mat4[asteroidCount];
		//glm::vec3 center = glm::vec3(0, -10, -50);
		//glm::vec3 dir = glm::vec3(1, 0, 0);
		//glm::vec3 pivot = glm::vec3(0, 1, 0);
		//
		//float radius = 100.0f;
		//float offset = 20.0f;
		//float offsetHeight = 10.0f;
		//for (unsigned int i = 0; i < asteroidCount; ++i)
		//{
		//	float height = offsetHeight * randMinOneToPlusOne();
		//	float r = radius + offset * randMinOneToPlusOne();
		//	glm::vec3 p = r * dir + height * pivot;
		//	float angle = randZeroToPlusOne() * 360.0f;
		//	float asteroidAngle = randZeroToPlusOne() * 360.0f;
		//
		//	float scaleWidth = randZeroToPlusOne() * 1.0f;
		//
		//	glm::vec3 scaleVec = glm::vec3(scaleWidth, scaleWidth, scaleWidth);
		//	glm::vec3 asteroidPivot = randomInUnitSphere();
		//
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, center);
		//	model = glm::rotate(model, glm::radians(angle), pivot);
		//	model = glm::translate(model, p);
		//	model = glm::scale(model, scaleVec);
		//	model = glm::rotate(model, glm::radians(asteroidAngle), asteroidPivot);
		//	modelMatrices[i] = model;
		//}
		//
		//unsigned int asteroidModelMatricesVbo;
		//glGenBuffers(1, &asteroidModelMatricesVbo);
		//glBindBuffer(GL_ARRAY_BUFFER, asteroidModelMatricesVbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * asteroidCount, &modelMatrices[0], GL_STATIC_DRAW);
		//
		//for (unsigned int i = 0; i < rockModel.meshes.size(); ++i)
		//{
		//	unsigned int VAO = rockModel.meshes[i].VAO;
		//	glBindVertexArray(VAO);
		//	//El maximo que puedes poner es 4 flotantes
		//	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		//	glEnableVertexAttribArray(3);
		//	glVertexAttribDivisor(3, 1);
		//
		//	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		//	glEnableVertexAttribArray(4);
		//	glVertexAttribDivisor(4, 1);
		//
		//	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		//	glEnableVertexAttribArray(5);
		//	glVertexAttribDivisor(5, 1);
		//
		//	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		//	glEnableVertexAttribArray(6);
		//	glVertexAttribDivisor(6, 1);
		//
		//	glBindVertexArray(0);
		//
		//}
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		// 
		// 
		//unsigned int shadDBTexture;
		//glGenTextures(1, &shadDBTexture);
		//glCheckError();
		//glBindTexture(GL_TEXTURE_2D, shadDBTexture);
		//glCheckError();
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadWidth, shadHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		//glCheckError();
		//
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glCheckError();
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glCheckError();
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glCheckError();
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		//glCheckError();
		//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		//NOTE: this is the resolution of the depthmap

//float quadVertices[]
		//{
		//	 0.5f, -0.5f, 0.0f, 1.0f,
		//	 0.5f, -1.0f, 0.0f, 0.0f,
		//	 1.0f, -1.0f, 1.0f, 0.0f,
		//
		//	 0.5f, -0.5f, 0.0f, 1.0f,
		//	 1.0f, -1.0f, 1.0f, 0.0f,
		//	 1.0f, -0.5f, 1.0f, 1.0f,
		//};
		// 
		// 
		//unsigned int cubeVao;
		//glGenVertexArrays(1, &cubeVao);
		//glBindVertexArray(cubeVao);
		//
		//unsigned int cubeVbo;
		//glGenBuffers(1, &cubeVbo);
		//glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		//
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(0);
		//
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);
		//
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//glEnableVertexAttribArray(2);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);

		//unsigned int quadVao;
		//glGenVertexArrays(1, &quadVao);
		//glBindVertexArray(quadVao);
		//
		//unsigned int quadVbo;
		//glGenBuffers(1, &quadVbo);
		//glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		//
		//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(0);
		//
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		//glEnableVertexAttribArray(1);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);

		const unsigned int shadWidth = 1024, shadHeight = 1024;


		unsigned int depthCubeMap;
		glGenTextures(1, &depthCubeMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
		unsigned int i = 0;
		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadWidth, shadHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		unsigned int shadDB;
		glGenFramebuffers(1, &shadDB);
		glCheckError();
		glBindFramebuffer(GL_FRAMEBUFFER, shadDB);
		glCheckError();

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
		glCheckError();

		glDrawBuffer(GL_NONE);
		glCheckError();
		glReadBuffer(GL_NONE);
		glCheckError();

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		else
			std::cout << "FRAMEBUFFER:: Framebuffer is complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCheckError();

		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		unsigned int cubeVAO;
		unsigned int cubeVBO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		unsigned int quadVAO;
		unsigned int quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));




		//NOTE: Uniform buffer objects!!!!!!!!!!!!!!

		for (Shader shader : list.shaders)
		{
			unsigned int bi = glGetUniformBlockIndex(shader.ID, "Matrices");
			glCheckError();
			if (bi != GL_INVALID_INDEX)
			{
				glUniformBlockBinding(shader.ID, bi, 0);
				glCheckError();
			}
		}

		unsigned int uboMatrices;
		glGenBuffers(1, &uboMatrices);
		glCheckError();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glCheckError();
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glCheckError();
		//NOTE: offset must be a multiple of GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0 * GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, 2 * sizeof(glm::mat4));
		glCheckError();
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glCheckError();

		for (Shader shader : list.shaders)
		{
			unsigned int bi = glGetUniformBlockIndex(shader.ID, "Lights");
			glCheckError();
			if (bi != GL_INVALID_INDEX)
			{
				glUniformBlockBinding(shader.ID, bi, 1);
				glCheckError();
			}
		}

		unsigned int uboLights;
		glGenBuffers(1, &uboLights);
		glCheckError();
		glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
		glCheckError();
		glBufferData(GL_UNIFORM_BUFFER, 20 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
		glCheckError();
		//NOTE: offset must be a multiple of GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboLights, 0 * GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, 20 * sizeof(GLfloat));
		glCheckError();
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glCheckError();


		float roomHalfWidth = 10.0f;
		float roomVertices[] = {
			//Bottom plane
			-roomHalfWidth,  0.0f, -roomHalfWidth,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			-roomHalfWidth,  0.0f,  roomHalfWidth,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
			 roomHalfWidth,  0.0f,  roomHalfWidth,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,

			-roomHalfWidth,  0.0f, -roomHalfWidth,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			 roomHalfWidth,  0.0f,  roomHalfWidth,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			 roomHalfWidth,  0.0f, -roomHalfWidth,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
			 //Top plane
			-roomHalfWidth, 10.0f, -roomHalfWidth,  0.0f,  -1.0f,  0.0f,  0.0f, 0.0f,
			 roomHalfWidth, 10.0f,  roomHalfWidth,  0.0f,  -1.0f,  0.0f,  1.0f, 1.0f,
			-roomHalfWidth, 10.0f,  roomHalfWidth,  0.0f,  -1.0f,  0.0f,  0.0f, 1.0f,

			-roomHalfWidth,  10.0f, -roomHalfWidth,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
			 roomHalfWidth,  10.0f, -roomHalfWidth,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			 roomHalfWidth,  10.0f,  roomHalfWidth,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,

			 //Right Plane
			-roomHalfWidth, 10.0f, roomHalfWidth,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-roomHalfWidth, 0.0f,  roomHalfWidth,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			-roomHalfWidth, 0.0f,  -roomHalfWidth,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

			-roomHalfWidth,  10.0f, roomHalfWidth,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
			-roomHalfWidth,  0.0f, -roomHalfWidth,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
			-roomHalfWidth,  10.0f, -roomHalfWidth, 1.0f, 0.0f,  0.0f,  1.0f, 1.0f,

			//Left Plane
			roomHalfWidth, 0.0f, -roomHalfWidth,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			roomHalfWidth, 0.0f,  roomHalfWidth,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			roomHalfWidth, 10.0f, -roomHalfWidth,  -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,

			roomHalfWidth,  10.0f, -roomHalfWidth,  -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
			roomHalfWidth,  0.0f,  roomHalfWidth,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
			roomHalfWidth,  10.0f, roomHalfWidth, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,

			//Back Plane
			-roomHalfWidth, 0.0f,  -roomHalfWidth,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
			roomHalfWidth, 0.0f,  -roomHalfWidth,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
			-roomHalfWidth, 10.0f, -roomHalfWidth,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

			-roomHalfWidth,  10.0f, -roomHalfWidth,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
			roomHalfWidth,  0.0f,  -roomHalfWidth,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
			roomHalfWidth,  10.0f, -roomHalfWidth,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f
		};

		unsigned int roomVao;
		glGenVertexArrays(1, &roomVao);
		glCheckError();
		glBindVertexArray(roomVao);
		glCheckError();
		unsigned int roomVbo;
		glGenBuffers(1, &roomVbo);
		glCheckError();
		glBindBuffer(GL_ARRAY_BUFFER, roomVbo);
		glCheckError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(roomVertices), roomVertices, GL_STATIC_DRAW);
		glCheckError();

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
		glCheckError();
		glEnableVertexAttribArray(0);
		glCheckError();
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glCheckError();
		glEnableVertexAttribArray(1);
		glCheckError();
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glCheckError();
		glEnableVertexAttribArray(2);
		glCheckError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glCheckError();
		glBindVertexArray(0);
		glCheckError();

		// positions
		glm::vec3 pos1(-1.0, 1.0, 0.0);
		glm::vec3 pos2(-1.0, -1.0, 0.0);
		glm::vec3 pos3(1.0, -1.0, 0.0);
		glm::vec3 pos4(1.0, 1.0, 0.0);
		// texture coordinates
		glm::vec2 uv1(0.0, 1.0);
		glm::vec2 uv2(0.0, 0.0);
		glm::vec2 uv3(1.0, 0.0);
		glm::vec2 uv4(1.0, 1.0);
		// normal vector
		glm::vec3 nm(0.0, 0.0, 1.0);

		glm::vec3 tangents[2];
		glm::vec3 bitangents[2];

		{
			//First triangle
			glm::vec3 edge0 = pos2 - pos1;
			glm::vec3 edge1 = pos3 - pos1;
			glm::vec2 deltaUV0 = uv2 - uv1;
			glm::vec2 deltaUV1 = uv3 - uv1;

			float coeff = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

			glm::vec3& tangent = tangents[0];
			glm::vec3& bitangent = bitangents[0];

			tangent.x = coeff * (deltaUV1.y * edge0.x - deltaUV0.y * edge1.x);
			tangent.y = coeff * (deltaUV1.y * edge0.y - deltaUV0.y * edge1.y);
			tangent.z = coeff * (deltaUV1.y * edge0.z - deltaUV0.y * edge1.z);

			bitangent.x = coeff * (-deltaUV1.x * edge0.x + deltaUV0.x * edge1.x);
			bitangent.y = coeff * (-deltaUV1.x * edge0.y + deltaUV0.x * edge1.y);
			bitangent.z = coeff * (-deltaUV1.x * edge0.z + deltaUV0.x * edge1.z);
		}

		{
			//Second triangle
			glm::vec3 edge0 = pos3 - pos1;
			glm::vec3 edge1 = pos4 - pos1;
			glm::vec2 deltaUV0 = uv3 - uv1;
			glm::vec2 deltaUV1 = uv4 - uv1;

			float coeff = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

			glm::vec3& tangent = tangents[1];
			glm::vec3& bitangent = bitangents[1];

			tangent.x = coeff * (deltaUV1.y * edge0.x - deltaUV0.y * edge1.x);
			tangent.y = coeff * (deltaUV1.y * edge0.y - deltaUV0.y * edge1.y);
			tangent.z = coeff * (deltaUV1.y * edge0.z - deltaUV0.y * edge1.z);

			bitangent.x = coeff * (-deltaUV1.x * edge0.x + deltaUV0.x * edge1.x);
			bitangent.y = coeff * (-deltaUV1.x * edge0.y + deltaUV0.x * edge1.y);
			bitangent.z = coeff * (-deltaUV1.x * edge0.z + deltaUV0.x * edge1.z);
		}


		float floorVertices[] = {
			//Bottom plane
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangents[0].x, tangents[0].y, tangents[0].z, bitangents[0].x, bitangents[0].y, bitangents[0].z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangents[0].x, tangents[0].y, tangents[0].z, bitangents[0].x, bitangents[0].y, bitangents[0].z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangents[0].x, tangents[0].y, tangents[0].z, bitangents[0].x, bitangents[0].y, bitangents[0].z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangents[1].x, tangents[1].y, tangents[1].z, bitangents[1].x, bitangents[1].y, bitangents[1].z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangents[1].x, tangents[1].y, tangents[1].z, bitangents[1].x, bitangents[1].y, bitangents[1].z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangents[1].x, tangents[1].y, tangents[1].z, bitangents[1].x, bitangents[1].y, bitangents[1].z
		};

		unsigned int floorVao;
		glGenVertexArrays(1, &floorVao);
		glCheckError();
		glBindVertexArray(floorVao);
		glCheckError();
		unsigned int floorVbo;
		glGenBuffers(1, &floorVbo);
		glCheckError();
		glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
		glCheckError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
		glCheckError();

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(0));
		glCheckError();
		glEnableVertexAttribArray(0);
		glCheckError();
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glCheckError();
		glEnableVertexAttribArray(1);
		glCheckError();
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glCheckError();
		glEnableVertexAttribArray(2);
		glCheckError();
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glCheckError();
		glEnableVertexAttribArray(3);
		glCheckError();
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
		glCheckError();
		glEnableVertexAttribArray(4);
		glCheckError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glCheckError();
		glBindVertexArray(0);
		glCheckError();

		//Texture object generation

		unsigned int txDiffWall;
		glGenTextures(1, &txDiffWall);
		glBindTexture(GL_TEXTURE_2D, txDiffWall);

		//set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCheckError();

		{
			stbi_set_flip_vertically_on_load(false);
			int width, height, chaCount;
			//TODO: Check how we are receiving the data!!!
			unsigned char* data = stbi_load("Source\\Textures\\crane_diffuse.png", &width, &height, &chaCount, 0);
			if (data)
			{
				//NOTE: SRGB 3rth parameter raises x^(1/2.2) to the power of 2.2; giving linear colors for diffuse textures only!!
				//NOTE: It should be used for diffuse textures only!!! NO specular maps, NO  height maps, etc.!!!
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glCheckError();
				glGenerateMipmap(GL_TEXTURE_2D);
				glCheckError();
				std::cout << "STB_IMAGE::LOADING_TEXTURE_SUCCESS" << std::endl;
			}
			else
			{
				std::cout << "ERROR::STB_IMAGE::FAILED TO LOAD TEXTURE" << std::endl;
			}
			stbi_image_free(data);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glCheckError();

		unsigned int txSpecWall;
		glGenTextures(1, &txSpecWall);
		glBindTexture(GL_TEXTURE_2D, txSpecWall);

		//set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCheckError();

		{
			stbi_set_flip_vertically_on_load(false);
			int width, height, chaCount;
			//TODO: Check how we are receiving the data!!!
			unsigned char* data = stbi_load("Source\\Textures\\crane_specular.png", &width, &height, &chaCount, 0);
			if (data)
			{
				GLenum format;
				if (chaCount == 1)
					format = GL_RED;
				else if (chaCount == 3)
					format = GL_RGB;
				else if (chaCount == 4)
					format = GL_RGBA;

				//NOTE: SRGB 3rth parameter raises x^(1/2.2) to the power of 2.2; giving linear colors for diffuse textures only!!
				//NOTE: It should be used for diffuse textures only!!! NO specular maps, NO  height maps, etc.!!!
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glCheckError();
				glGenerateMipmap(GL_TEXTURE_2D);
				glCheckError();
				std::cout << "STB_IMAGE::LOADING_TEXTURE_SUCCESS" << std::endl;
			}
			else
			{
				std::cout << "ERROR::STB_IMAGE::FAILED TO LOAD TEXTURE" << std::endl;
			}
			stbi_image_free(data);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glCheckError();

		unsigned int txNormWall;
		glGenTextures(1, &txNormWall);
		glBindTexture(GL_TEXTURE_2D, txNormWall);

		//set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCheckError();

		{
			stbi_set_flip_vertically_on_load(false);
			int width, height, chaCount;
			//TODO: Check how we are receiving the data!!!
			unsigned char* data = stbi_load("Source\\Textures\\brickwall_normal.jpg", &width, &height, &chaCount, 0);
			if (data)
			{
				GLenum format;
				if (chaCount == 1)
					format = GL_RED;
				else if (chaCount == 3)
					format = GL_RGB;
				else if (chaCount == 4)
					format = GL_RGBA;

				//NOTE: SRGB 3rth parameter raises x^(1/2.2) to the power of 2.2; giving linear colors for diffuse textures only!!
				//NOTE: It should be used for diffuse textures only!!! NO specular maps, NO  height maps, etc.!!!
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glCheckError();
				glGenerateMipmap(GL_TEXTURE_2D);
				glCheckError();
				std::cout << "STB_IMAGE::LOADING_TEXTURE_SUCCESS" << std::endl;
			}
			else
			{
				std::cout << "ERROR::STB_IMAGE::FAILED TO LOAD TEXTURE" << std::endl;
			}
			stbi_image_free(data);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glCheckError();

		unsigned int txDispWall;
		glGenTextures(1, &txDispWall);
		glBindTexture(GL_TEXTURE_2D, txDispWall);

		//set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCheckError();

		{
			stbi_set_flip_vertically_on_load(false);
			int width, height, chaCount;
			//TODO: Check how we are receiving the data!!!
			unsigned char* data = stbi_load("Source\\Textures\\wood_height.png", &width, &height, &chaCount, 0);
			if (data)
			{
				GLenum format;
				if (chaCount == 1)
					format = GL_RED;
				else if (chaCount == 3)
					format = GL_RGB;
				else if (chaCount == 4)
					format = GL_RGBA;

				//NOTE: SRGB 3rth parameter raises x^(1/2.2) to the power of 2.2; giving linear colors for diffuse textures only!!
				//NOTE: It should be used for diffuse textures only!!! NO specular maps, NO  height maps, etc.!!!
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glCheckError();
				glGenerateMipmap(GL_TEXTURE_2D);
				glCheckError();
				std::cout << "STB_IMAGE::LOADING_TEXTURE_SUCCESS" << std::endl;
			}
			else
			{
				std::cout << "ERROR::STB_IMAGE::FAILED TO LOAD TEXTURE" << std::endl;
			}
			stbi_image_free(data);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glCheckError();


		///////////////////////////////////////////////////////////////////////////////////////////////////
		//NOTE: HDR FrameBuffer

		FrameBuffer gBuffer;
		{
			const unsigned int ColorBufferCount = 3;
			const unsigned int DepthBufferCount = 1;
			gBuffer.colorBuffers = std::make_unique<Buffer2D[]>(ColorBufferCount);
			gBuffer.depthBuffers = std::make_unique<Buffer2D[]>(DepthBufferCount);

			glGenFramebuffers(1, &gBuffer.id);
			glCheckError();
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.id);
			glCheckError();

			//Positions texture

			gBuffer.colorBuffers[GBUFFER_TEX_INDEX_POSITON].height = 720;
			gBuffer.colorBuffers[GBUFFER_TEX_INDEX_POSITON].width = 1280;

			glGenTextures(1, &gBuffer.colorBuffers[GBUFFER_TEX_INDEX_POSITON].id);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_POSITON].id);
			glCheckError();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_POSITON].width, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_POSITON].height, 0, GL_RGBA, GL_FLOAT, NULL);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, 0);
			glCheckError();

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_POSITON].id, 0);
			glCheckError();

			//Normals texture

			gBuffer.colorBuffers[GBUFFER_TEX_INDEX_NORMAL].height = 720;
			gBuffer.colorBuffers[GBUFFER_TEX_INDEX_NORMAL].width = 1280;

			glGenTextures(1, &gBuffer.colorBuffers[GBUFFER_TEX_INDEX_NORMAL].id);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_NORMAL].id);
			glCheckError();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_NORMAL].width, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_NORMAL].height, 0, GL_RGBA, GL_FLOAT, NULL);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, 0);
			glCheckError();

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_NORMAL].id, 0);
			glCheckError();

			//Albedo + spec texture

			gBuffer.colorBuffers[GBUFFER_TEX_INDEX_COLORSPEC].height = 720;
			gBuffer.colorBuffers[GBUFFER_TEX_INDEX_COLORSPEC].width = 1280;

			glGenTextures(1, &gBuffer.colorBuffers[GBUFFER_TEX_INDEX_COLORSPEC].id);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_COLORSPEC].id);
			glCheckError();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_COLORSPEC].width, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_COLORSPEC].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, 0);
			glCheckError();

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_COLORSPEC].id, 0);
			glCheckError();


			//Depth renderbuffer 

			gBuffer.depthBuffers[0].width = 1280;
			gBuffer.depthBuffers[0].height = 720;

			glGenRenderbuffers(1, &gBuffer.depthBuffers[0].id);
			glCheckError();
			glBindRenderbuffer(GL_RENDERBUFFER, gBuffer.depthBuffers[0].id);
			glCheckError();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gBuffer.depthBuffers[0].width, gBuffer.depthBuffers[0].height);
			glCheckError();
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glCheckError();

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBuffer.depthBuffers[0].id);
			glCheckError();

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			else
				std::cout << "FRAMEBUFFER:: Framebuffer is complete!" << std::endl;

			unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(3, attachments);
			glCheckError();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glCheckError();
		}


		///////////////////////////////////////////////////////////////////////////////////////////////////
		//NOTE: Ping Pong buffers!
		const unsigned int FrameBufferCount = 2;
		FrameBuffer PingPongBuffers[FrameBufferCount];
		const unsigned int ColorBufferCount = 1;
		PingPongBuffers[0].colorBuffers = std::make_unique<Buffer2D[]>(ColorBufferCount);
		PingPongBuffers[1].colorBuffers = std::make_unique<Buffer2D[]>(ColorBufferCount);

		for (unsigned int i = 0; i < FrameBufferCount; ++i)
		{
			glGenFramebuffers(1, &PingPongBuffers[i].id);
			glCheckError();
			glBindFramebuffer(GL_FRAMEBUFFER, PingPongBuffers[i].id);
			glCheckError();

			PingPongBuffers[i].colorBuffers[0].height = 720;
			PingPongBuffers[i].colorBuffers[0].width = 1280;

			glGenTextures(1, &PingPongBuffers[i].colorBuffers[0].id);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, PingPongBuffers[i].colorBuffers[0].id);
			glCheckError();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, PingPongBuffers[i].colorBuffers[0].width, PingPongBuffers[i].colorBuffers[0].height, 0, GL_RGBA, GL_FLOAT, NULL);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glCheckError();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, 0);
			glCheckError();

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PingPongBuffers[i].colorBuffers[0].id, 0);
			glCheckError();

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			else
				std::cout << "FRAMEBUFFER:: Framebuffer is complete!" << std::endl;

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glCheckError();

		}



		///////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<glm::vec3> lightPositions;
		lightPositions.push_back(glm::vec3(0.0f, 0.0f, -17.0f)); // back light
		lightPositions.push_back(glm::vec3(2.4f, 0.0f, -12.0f));
		lightPositions.push_back(glm::vec3(-2.4f, 0.0f, -9.0f));
		lightPositions.push_back(glm::vec3(2.4f, -2.4f, -7.0f));
		// colors
		std::vector<glm::vec3> lightColors;
		lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
		lightColors.push_back(glm::vec3(1.1f, 0.0f, 0.0f));
		lightColors.push_back(glm::vec3(0.0f, 1.1f, 0.0f));
		lightColors.push_back(glm::vec3(0.0f, 0.0f, 1.2f));

		glm::mat4 originalProjection = camera.getProjectionMatrix();

		bool horizontal = true;
		unsigned int blurIterations = 10;

		while (!glfwWindowShouldClose(window))
		{
			process_input(window);
			glm::vec3 lightPos = glm::vec3(0.0f, -2.0f, 0.0f);

			glm::mat4 lightModelMatrix = glm::mat4(1.0f);
			lightModelMatrix = glm::translate(lightModelMatrix, lightPos);
			lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(1.0, 1.0, 1.0));

			glm::mat4 floorModelMatrix = glm::mat4(1.0f);
			floorModelMatrix = glm::translate(floorModelMatrix, glm::vec3(0.0f, -5.0f, 0.0f));
			floorModelMatrix = glm::rotate(floorModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			floorModelMatrix = glm::scale(floorModelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));

			glm::mat4 roomModelMatrix = glm::mat4(1.0f);
			roomModelMatrix = glm::translate(roomModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
			roomModelMatrix = glm::scale(roomModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));


			glm::mat4 tunnelModelMatrix = glm::mat4(1.0f);
			tunnelModelMatrix = glm::translate(tunnelModelMatrix, glm::vec3(0.0, 0.0, 0.0));
			tunnelModelMatrix = glm::scale(tunnelModelMatrix, glm::vec3(2.5f, 2.5f, 20.0f));

			glm::mat4 projection = camera.getProjectionMatrix();
			glm::mat4 view = camera.getViewMatrix();

			glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
			glCheckError();

			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
			glCheckError();

			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
			glCheckError();

			//glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightOrthoMatrix * lightViewMatrix));
			//glCheckError();

			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glCheckError();

			GLintptr N = sizeof(GLfloat);

			glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
			glCheckError();

			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(lightPos));
			glCheckError();

			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
			glCheckError();

			glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec3(5.0, 5.0, 5.0)));
			glCheckError();

			glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
			glCheckError();

			glm::vec3 clq = glm::vec3(1.0f, 0.027f, 0.0028f);
			//glm::vec3 clq = glm::vec3(1.0f, 0.35f, 0.44f);

			glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(clq));
			glCheckError();

			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glCheckError();

			//1st Pass Render HDR framebuffer color attachments
			list.shaders[0].use();
			
			//NOTE: Here we are using the first color buffer dimensions only!!
			glViewport(0, 0, gBuffer.colorBuffers[0].width, gBuffer.colorBuffers[0].height);
			glCheckError();
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.id);
			glCheckError();
			glEnable(GL_DEPTH_TEST);
			glCheckError();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glCheckError();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCheckError();

			list.shaders[0].setMat4("modelMatrix", roomModelMatrix);

			bpModel.Draw(list.shaders[0]);

			//2nd pass deferred shading
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glCheckError();
			glClearColor(0, 0, 0, 1.0f);
			glCheckError();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCheckError();
			
			glViewport(0, 0, viewPortWidth, viewPortHeight);
			glCheckError();
			
			list.shaders[1].use();
			
			list.shaders[1].setInt("tex", 0);
			glActiveTexture(GL_TEXTURE0);
			glCheckError();
			glBindTexture(GL_TEXTURE_2D, gBuffer.colorBuffers[GBUFFER_TEX_INDEX_COLORSPEC].id);
			glCheckError();
			
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);


			///////////////////////////////////////////////////////////////////////////////////////////////////
			// Shadow mapping to 2D texture
			//glm::mat4 lightRotateModel = glm::mat4(1.0f);
			//lightRotateModel = glm::rotate(lightRotateModel, 10.0f * glm::radians((float)glfwGetTime()), glm::vec3(0, 1, 0));
			//lightRotateModel = glm::translate(lightRotateModel, glm::vec3(4.0, 4.0, 4.0));
			/////////////////////////////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////////////////////////////////////
			////1st Pass Render HDR framebuffer color attachments
			//list.shaders[0].use();
			//
			////NOTE: Here we are using the first color buffer dimensions only!!
			//glViewport(0, 0, hdrFB.colorBuffers[0].width, hdrFB.colorBuffers[0].height);
			//glCheckError();
			//glBindFramebuffer(GL_FRAMEBUFFER, hdrFB.id);
			//glCheckError();
			//glEnable(GL_DEPTH_TEST);
			//glCheckError();
			//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			//glCheckError();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glCheckError();
			//
			////list.shaders[0].setInt("shadowMap", 0);
			////glActiveTexture(GL_TEXTURE0);
			////glCheckError();
			////glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
			////glCheckError();
			////list.shaders[0].setFloat("far", far);
			//
			//list.shaders[0].setVec3("viewPos", camera.getCameraPos());
			//list.shaders[0].setFloat("mat.shininess", 32.0f);
			//
			////list.shaders[0].setFloat("specularColor", 0.3f);
			//list.shaders[0].setFloat("specularColor", 1.0f);
			//
			//list.shaders[0].setInt("mat.texture_diffuse1", 0);
			//glActiveTexture(GL_TEXTURE0);
			//glCheckError();
			//glBindTexture(GL_TEXTURE_2D, txDiffWall);
			//glCheckError();
			//
			//list.shaders[0].setInt("mat.texture_normal1", 1);
			//glActiveTexture(GL_TEXTURE1);
			//glCheckError();
			//glBindTexture(GL_TEXTURE_2D, txNormWall);
			//glCheckError();
			//
			//list.shaders[0].setMat4("modelMatrix", floorModelMatrix);
			//
			//glBindVertexArray(floorVao);
			//glDrawArrays(GL_TRIANGLES, 0, 6);
			//glBindVertexArray(0);
			//
			//list.shaders[1].use();
			//list.shaders[1].setMat4("modelMatrix", lightModelMatrix);
			//
			//glBindVertexArray(cubeVAO);
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			//glBindVertexArray(0);
			//
			/////////////////////////////////////////////////////////////////////////////////////////////////////
			////2nd Pass Gaussian blur
			////NOTE: Here you don´t clear color, because you need the data.
			//bool firstIteration = true;
			//list.shaders[3].use();
			//for (unsigned int i = 0; i < blurIterations; ++i)
			//{
			//	glBindFramebuffer(GL_FRAMEBUFFER, PingPongBuffers[horizontal].id);
			//	glCheckError();
			//
			//	list.shaders[3].setBool("horizontal", horizontal);
			//	list.shaders[3].setInt("image", 0);
			//	glActiveTexture(GL_TEXTURE0);
			//	glCheckError();
			//	glBindTexture(GL_TEXTURE_2D, firstIteration ? hdrFB.colorBuffers[1].id : PingPongBuffers[!horizontal].colorBuffers[0].id);
			//	glCheckError();
			//
			//	glBindVertexArray(quadVAO);
			//	glCheckError();
			//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			//	glCheckError();
			//	glBindVertexArray(0);
			//	glCheckError();
			//
			//	if (firstIteration) firstIteration = false;
			//	horizontal = !horizontal;
			//}
			/////////////////////////////////////////////////////////////////////////////////////////////////////
			////3rd Pass Default FrameBuffer
			//
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glCheckError();
			//glClearColor(0, 0, 0, 1.0f);
			//glCheckError();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glCheckError();
			//
			//glViewport(0, 0, viewPortWidth, viewPortHeight);
			//glCheckError();
			//
			//list.shaders[2].use();
			//
			//list.shaders[2].setFloat("exposure", exposure);
			//
			//list.shaders[2].setInt("sceneBuffer", 0);
			//glActiveTexture(GL_TEXTURE0);
			//glCheckError();
			//glBindTexture(GL_TEXTURE_2D, hdrFB.colorBuffers[0].id);
			//glCheckError();
			//
			//list.shaders[2].setInt("bloomBuffer", 1);
			//glActiveTexture(GL_TEXTURE1);
			//glCheckError();
			//glBindTexture(GL_TEXTURE_2D, PingPongBuffers[1].colorBuffers[0].id);
			//glCheckError();
			//
			//glBindVertexArray(quadVAO);
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			//glBindVertexArray(0);

			///////////////////////////////////////////////////////////////////////////////////////////////////

			// 
			//HDR rendering within cube with 3 bright lights!!!
			//glBindFramebuffer(GL_FRAMEBUFFER, hdrFB.id);
			//glCheckError();
			//glEnable(GL_DEPTH_TEST);
			//glCheckError();
			//glClearColor(0, 0, 0, 1.0f);
			//glCheckError();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glCheckError();
			//
			//list.shaders[0].use();
			//
			//
			//list.shaders[0].setInt("diffuseTexture", 0);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, txDiffWall);
			//
			//// set lighting uniforms
			//for (unsigned int i = 0; i < lightPositions.size(); i++)
			//{
			//	list.shaders[0].setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			//	list.shaders[0].setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
			//}
			//
			//list.shaders[0].setVec3("viewPos", camera.getCameraPos());
			//
			//list.shaders[0].setInt("inverse_normals", true);
			//list.shaders[0].setMat4("model", tunnelModelMatrix);
			//glBindVertexArray(cubeVAO);
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			//glBindVertexArray(0);
			//
			//glViewport(0, 0, viewPortWidth, viewPortHeight);
			//
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glCheckError();
			//glClearColor(0, 0, 0, 1.0f);
			//glCheckError();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glCheckError();
			//
			//list.shaders[1].use();
			//
			//list.shaders[1].setBool("hdr", hdr);
			//list.shaders[1].setFloat("exposure", exposure);
			//
			//list.shaders[1].setInt("hdrBuffer", 0);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, hdrFB.texColorBufferID);
			//
			//glBindVertexArray(quadVAO);
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			//glBindVertexArray(0);




			/////////////////////////////////////////////////////////////////////////////////////////////////////
			////Omnidirectional Shadow Mapping
			//glm::vec3 lightPos = glm::vec3(0.0f, 90.0f, 0.0f);
			//float aspect = 1.0f;
			//float near = 1.0f;
			//float far = 250.0f;
			//glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
			//
			//std::vector<glm::mat4> shadowTransforms;
			////right
			//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			////left
			//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			////top
			//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
			////bottom
			//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
			////near
			//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			////far
			//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
			//
			//
			//
			//glm::mat4 projection = camera.getProjectionMatrix();
			//glm::mat4 view = camera.getViewMatrix();
			//
			//glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
			//glCheckError();
			//
			//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
			//glCheckError();
			//
			//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
			//glCheckError();
			//
			////glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightOrthoMatrix * lightViewMatrix));
			////glCheckError();
			//
			//glBindBuffer(GL_UNIFORM_BUFFER, 0);
			//glCheckError();
			//
			//
			//glm::mat4 roomModelMatrix = glm::mat4(1.0f);
			//roomModelMatrix = glm::translate(roomModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
			//roomModelMatrix = glm::scale(roomModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
			//
			//glm::mat4 cubeModelMatrix0 = glm::mat4(1.0f);
			//cubeModelMatrix0 = glm::translate(cubeModelMatrix0, glm::vec3(-5.0f, -3.0f, 0.0f));
			////cubeModelMatrix0 = glm::scale(cubeModelMatrix0, glm::vec3(1.0f, 50.0f, 50.0f));
			//
			//glm::mat4 cubeModelMatrix1 = glm::mat4(1.0f);
			//cubeModelMatrix1 = glm::translate(cubeModelMatrix1, glm::vec3(4.0f, 0.0f, 0.0f));
			//cubeModelMatrix1 = glm::scale(cubeModelMatrix1, glm::vec3(2.0f, 2.0f, 2.0f));
			//
			//glViewport(0, 0, shadWidth, shadHeight);
			//glCheckError();
			//glBindFramebuffer(GL_FRAMEBUFFER, shadDB);
			//glCheckError();
			//glEnable(GL_DEPTH_TEST);
			//glCheckError();
			//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			//glCheckError();
			//glClear(GL_DEPTH_BUFFER_BIT);
			//glCheckError();
			//
			//list.shaders[1].use();
			//
			//for (int i = 0; i < shadowTransforms.size(); ++i)
			//{
			//	list.shaders[1].setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
			//}
			//
			//list.shaders[1].setVec3("lightPos", lightPos);
			//list.shaders[1].setFloat("far_plane", far);
			//
			//list.shaders[1].setMat4("modelMatrix", roomModelMatrix);
			//bpModel.Draw(list.shaders[1]);
			//
			////list.shaders[1].setMat4("modelMatrix", roomModelMatrix);
			////
			////glBindVertexArray(floorVao);
			////glDrawArrays(GL_TRIANGLES, 0, 30);
			////glBindVertexArray(0);
			////
			////list.shaders[1].setMat4("modelMatrix", cubeModelMatrix0);
			////
			////glBindVertexArray(cubeVao);
			////glDrawArrays(GL_TRIANGLES, 0, 36);
			////glBindVertexArray(0);
			////
			////
			////list.shaders[1].setMat4("modelMatrix", cubeModelMatrix1);
			////
			////glBindVertexArray(cubeVao);
			////glDrawArrays(GL_TRIANGLES, 0, 36);
			////glBindVertexArray(0);
			//
			//list.shaders[0].use();
			//glViewport(0, 0, viewPortWidth, viewPortHeight);
			//glCheckError();
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glCheckError();
			//
			//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			//glCheckError();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glCheckError();
			//
			//list.shaders[0].setInt("shadowMap", 0);
			//glActiveTexture(GL_TEXTURE0);
			//glCheckError();
			//glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
			//glCheckError();
			//list.shaders[0].setVec3("viewPos", camera.getCameraPos());
			//
			//list.shaders[0].setVec3("pLight.position", lightPos);
			//list.shaders[0].setVec3("pLight.ambient", glm::vec3(0.1, 0.1, 0.1));
			//list.shaders[0].setVec3("pLight.diffuse", glm::vec3(0.5, 0.5, 0.5));
			//list.shaders[0].setVec3("pLight.specular", glm::vec3(1.0, 1.0, 1.0));
			//
			////NOTE:Alex 65 meters falloff
			//list.shaders[0].setFloat("pLight.c", 1.0f);
			//list.shaders[0].setFloat("pLight.l", 0.027f);
			//list.shaders[0].setFloat("pLight.q", 0.0028f);
			//
			////list.shaders[0].setFloat("specularColor", 1.0f);
			//list.shaders[0].setFloat("mat.shininess", 64.0f);
			//list.shaders[0].setFloat("far", far);
			//
			////list.shaders[0].setInt("mat.texture_diffuse1", 1);
			////glActiveTexture(GL_TEXTURE1);
			////glCheckError();
			////glBindTexture(GL_TEXTURE_2D, txWood);
			////glCheckError();
			//
			//list.shaders[0].setMat4("modelMatrix", roomModelMatrix);
			//bpModel.Draw(list.shaders[0]);

			/////////////////////////////////////////////////////////////////////////////////////////////////////

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		//DeleteFrameBuffer();
	}
	else
	{
		int x = 5;
	}
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
