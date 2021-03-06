#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

extern float calcDeltaTime();
extern bool activeFocus;

class Camera
{
public:
	Camera(int viewPortWidth, int viewPortHeight);
	void scroll(double xOffset, double yOffset);
	void move(GLFWwindow* window);
	glm::mat4 getProjectionMatrix();
	
	void setViewPort(int viewPortWidth, int viewPortHeight);

	glm::mat4 getViewMatrix();
	void rotate(double mouseXPD, double mouseYPD);
	void setFirstMouse(bool value);
	glm::vec3 getCameraPos();
	glm::vec3 getCameraFront();
	float getNearPlane();
	float getFarPlane();
private:
	
	int viewPortWidth;
	int viewPortHeight;
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 up;
	glm::vec3 camZ;
	glm::vec3 camX;
	glm::vec3 camY;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;
	float cameraSpeed = 10.0f;
	//float cameraSpeed = 10.0f;
	float lastMouseXP;
	float lastMouseYP;
	float yaw;
	float pitch;
	bool firstMouse;
	float fov;
	float nearPlane;
	float farPlane;
};

