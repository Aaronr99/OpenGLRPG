#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../GameObject.h" 
#include <vector>
#include "../GlobalData.h"
#include <string>

struct LightObject
{
	int lightID;
	std::vector<float> vertices;
	unsigned int* lightVao, * lightVBO;
	Shader objectShader;
	Shader* lightShader;
	Transform transform;

	LightObject(const Transform& transform, Shader& shader, Shader* shader2,std::vector<float>  vertices, unsigned int* VAO, unsigned int* VBO, int id)
	{
		this->transform = transform;
		lightVao = VAO;
		lightVBO = VBO;
		this->vertices = vertices;
		this->objectShader = shader;
		this->lightID = id;
		lightShader = shader2;
	}

	LightObject(std::vector<float>  vertices, Shader& shader, Shader* shader2, unsigned int* VAO, unsigned int* VBO, int id)
	{
		this->transform = Transform();
		lightVao = VAO;
		lightVBO = VBO;
		this->vertices = vertices;
		this->objectShader = shader;
		this->lightID = id;
		lightShader = shader2;
	}

	void Awake();
	void Update();
	void Render();
	//LightObject(Model& model, Shader& shader) : GameObject(model, shader) {}
};

