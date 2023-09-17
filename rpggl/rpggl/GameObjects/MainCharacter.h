#pragma once
#include "../GameObject.h"
#include "../InputManager.h"
#include "../camera.h"


struct MainCharacter : GameObject
{
	Camera* camera;
	MainCharacter(const Transform& transform, const Renderer& renderer, Camera* pCamera) : GameObject(transform, renderer) 
	{
		camera = pCamera;
	}
	MainCharacter(const Renderer& renderer, Camera* pCamera) : GameObject(renderer) 
	{
		camera = pCamera;
	}
	MainCharacter(Model& model, Shader& shader, Camera* pCamera) : GameObject(model, shader) 
	{
		camera = pCamera;
	}

	void Update() override;
};

