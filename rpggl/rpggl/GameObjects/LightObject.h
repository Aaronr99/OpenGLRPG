#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../GameObject.h" 
#include <vector>

struct LightObject
{
	std::vector<float> vertices;
	unsigned int* lightVao, * lightVBO;
	Shader shader;
	Transform transform;

	LightObject(const Transform& transform, Shader& shader, std::vector<float>  vertices, unsigned int* VAO, unsigned int* VBO)
	{
		this->transform = transform;
		lightVao = VAO;
		lightVBO = VBO;
		this->vertices = vertices;
		this->shader = shader;
	}
	LightObject(std::vector<float>  vertices, Shader& shader, unsigned int* VAO, unsigned int* VBO)
	{
		this->transform = Transform();
		lightVao = VAO;
		lightVBO = VBO;
		this->vertices = vertices;
		this->shader = shader;
	}

	void Awake();
	void Render();
	//LightObject(Model& model, Shader& shader) : GameObject(model, shader) {}
};

