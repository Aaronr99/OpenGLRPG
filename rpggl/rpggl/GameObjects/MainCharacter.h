#pragma once
#include "../GameObject.h"
#include "../InputManager.h"
#include "../camera.h"
#include "../GlobalData.h"


struct MainCharacter : GameObject
{
	Camera* camera;
	bool isMoving;
	glm::vec3 targetPos;

	MainCharacter(const Transform& transform, const Renderer& renderer, Camera* pCamera) : GameObject(transform, renderer) 
	{
		camera = pCamera;
		targetPos = glm::vec3(0.0f);
		isMoving = false;
	}
	MainCharacter(const Renderer& renderer, Camera* pCamera) : GameObject(renderer) 
	{
		camera = pCamera;
		targetPos = glm::vec3(0.0f);
		isMoving = false;
	}
	MainCharacter(Model& model, Shader& shader, Camera* pCamera) : GameObject(model, shader) 
	{
		camera = pCamera;
		targetPos = glm::vec3(0.0f);
		isMoving = false;
	}

	void Update() override;
};

