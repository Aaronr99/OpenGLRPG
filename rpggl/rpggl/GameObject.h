#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "Model.h"

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 matrix;

	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
		: position(position), rotation(rotation), scale(scale) 
	{
		matrix = glm::mat4(1.0f);
		Actualize();
	}

	Transform() {
		position = glm::vec3(0.0f);
		rotation = glm::vec3(1.0f);
		scale = glm::vec3(1.0f);
		Actualize();
	}

	void Actualize();
	void Move(glm::vec3 destiny);
	void Rotate(glm::vec3 destiny);
	void Scale(glm::vec3 destiny);
};

struct Renderer
{
	Model model;
	Shader shader;

	Renderer(const Model& model, const Shader& shader)
		: model(model), shader(shader) {}

	Renderer() = default;
};

struct GameObject {
	Transform transform;
	Renderer renderer;

	GameObject(const Transform& transform, const Renderer& renderer)
		: transform(transform), renderer(renderer) {}

	GameObject(const Renderer& renderer) : renderer(renderer)
	{
		transform = Transform();
	}

	GameObject(Model& model, Shader& shader)
		: renderer(model, shader), transform() {}

	virtual void Update();
	virtual void Render();

	/*void Move(glm::vec3 destiny, float time);
	void Rotate(glm::vec3 destiny, float time);
	void Scale(glm::vec3 destiny, float time);*/
};
