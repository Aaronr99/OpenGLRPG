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
#include "../GlobalData.h"


struct CameraManager
{
	Shader shader;
	Transform* targetTransform;
	Transform previousTransform;
	glm::vec3 offsetToTarget;
	float horizontalAngle;

	CameraManager(Shader& pShader, Transform* pTarget) 
	{
		shader = pShader;
		targetTransform = pTarget;
		previousTransform = Transform(*pTarget);
		offsetToTarget = previousTransform.position - GlobalData::camera.Position;
		horizontalAngle = 0.0f;
	}

	void Update();
	void Render();
};




