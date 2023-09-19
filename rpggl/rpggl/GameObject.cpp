#include "GameObject.h"

void  Transform::Actualize()
{
	matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, position);
	matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	matrix = glm::scale(matrix, scale);
}

void Transform::Move(glm::vec3 destiny)
{
	position = destiny;
	Actualize();
}

void Transform::Rotate(glm::vec3 destiny)
{
	rotation = destiny;
	Actualize();
}

void Transform::Scale(glm::vec3 destiny)
{
	scale = destiny;
	Actualize();
}

void GameObject::Awake()
{

}

void GameObject::Update()
{

}

void GameObject::Render()
{
	renderer.shader.setMat4("model", transform.matrix);
	renderer.model.Draw(renderer.shader);
}