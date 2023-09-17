#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../camera.h"
#include "../Shader.h"
#include "../GameObject.h"
#include "../InputManager.h"


struct CameraManager
{
	unsigned int width;
	unsigned int height;
	Camera* camera;
	Shader shader;
	Transform* targetTransform;
	Transform previousTransform;
	glm::vec3 offsetToTarget;
	float horizontalAngle;

	CameraManager(Shader& pShader, Camera* pCamera, int pWidth, int pHeight, Transform* pTarget) 
	{
		camera = pCamera;
		width = pWidth;
		height = pHeight;
		shader = pShader;
		targetTransform = pTarget;
		previousTransform = Transform(*pTarget);
		offsetToTarget = previousTransform.position - camera->Position;
		horizontalAngle = 0.0f;
	}

	void Update(float deltaTime);
	void Render();
};




