#pragma once
#include "../GameObject.h"
#include "../InputManager.h"

struct MainCharacter : GameObject
{
	MainCharacter(const Transform& transform, const Renderer& renderer) : GameObject(transform, renderer) {}
	MainCharacter(const Renderer& renderer) : GameObject(renderer) {}
	MainCharacter(Model& model, Shader& shader) : GameObject(model, shader) {}

	void Update() override;
};

