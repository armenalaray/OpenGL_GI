#include "Camera.h"

Camera::Camera(int viewPortWidth_, int viewPortHeight_)
{
	viewPortWidth = viewPortWidth_;
	viewPortHeight = viewPortHeight_;

	////Gram-Schmidt process
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraTarget = glm::vec3(0, 0, 0);
	up = glm::vec3(0, 1.f, 0);

	camZ = glm::normalize(cameraPos - cameraTarget);
	camX = glm::normalize(glm::cross(up, camZ));
	camY = glm::cross(camZ, camX);

	cameraFront = -camZ;
	cameraRight = camX;

	lastMouseXP = ((float)viewPortWidth * 0.5f);
	lastMouseYP = ((float)viewPortHeight * 0.5f);

	yaw = -90.0f;
	pitch = 0.0f;

	firstMouse = true;

	fov = 45.0f;
	nearPlane = 0.1f;
	farPlane = 1000.0f;
}



//NOTE: xpos and ypos are screen pixel coordinates. 800 width * 600 height.
void Camera::rotate(double mouseXPD, double mouseYPD)
{
	if (activeFocus)
	{
		float mouseXP = (float)mouseXPD;
		float mouseYP = (float)mouseYPD;

		if (firstMouse)
		{
			lastMouseXP = mouseXP;
			lastMouseYP = mouseYP;
			firstMouse = false;
		}

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

		glm::vec3 dir;
		dir.x = cosf(glm::radians(pitch)) * cosf(glm::radians(yaw));
		dir.y = sinf(glm::radians(pitch));
		dir.z = cosf(glm::radians(pitch)) * sinf(glm::radians(yaw));
		cameraFront = glm::normalize(dir);

		camZ = -cameraFront;
		camX = glm::normalize(glm::cross(up, camZ));
		camY = glm::cross(camZ, camX);

		cameraRight = camX;
	}
}

void Camera::setFirstMouse(bool value)
{
	firstMouse = value;
}

glm::vec3 Camera::getCameraPos()
{
	return cameraPos;
}

glm::vec3 Camera::getCameraFront()
{
	return cameraFront;
}

float Camera::getNearPlane()
{
	return nearPlane;
}

float Camera::getFarPlane()
{
	return farPlane;
}

void Camera::scroll(double xOffset, double yOffset)
{
	if(activeFocus)
	{
		fov -= (float)yOffset;
		if (fov < 1.0f) fov = 1.0f;
		if (fov > 45.0f) fov = 45.0f;
	}
}

void Camera::move(GLFWwindow* window)
{
	if (activeFocus)
	{
		float unitsToAdvance = cameraSpeed * calcDeltaTime();
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += unitsToAdvance * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos += -(unitsToAdvance * cameraRight);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos += -(unitsToAdvance * cameraFront);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += unitsToAdvance * cameraRight;
	}
}

glm::mat4 Camera::getProjectionMatrix()
{
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(fov), (float)viewPortWidth / (float)viewPortHeight, nearPlane, farPlane);
	return projection;
}

glm::mat4 Camera::getViewMatrix()
{
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
	return view;
}
