#ifndef CAMERA_H
#define CAMERA_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

const float RADIUS = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::vec3 Target;
	float MouseSensitivity;
	float Zoom;
	float Radius;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), Radius(RADIUS)
	{
		Position = position;
		WorldUp = up;
		Target = target;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void SetTarget(glm::vec3 target)
	{
		Target = target;
		OrbitAroundTarget(target, Radius, 0.0f);
	}

	void SetTarget(glm::vec3 target, float horizontalAngle)
	{
		Target = target;
		OrbitAroundTarget(target, Radius, horizontalAngle);
	}

	void OrbitAroundTarget(glm::vec3 target, float radius, float horizontalAngle)
	{
		// Calcula la nueva posición de la cámara en función del objetivo y los ángulos proporcionados.
		float camX = target.x + static_cast<float>(radius * sin(glm::radians(horizontalAngle)));
		float camY = Position.y;
		float camZ = target.z + static_cast<float>(radius * cos(glm::radians(horizontalAngle)));

		Position = glm::vec3(camX, camY, camZ);

		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	void updateCameraVectors()
	{
		Front = glm::normalize(Target - Position);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif