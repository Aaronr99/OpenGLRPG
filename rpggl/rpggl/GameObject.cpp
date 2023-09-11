#include "GameObject.h"

void GameObject::Update() 
{

}

void GameObject::Render() 
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::scale(model, transform.scale);
	renderer.shader.setMat4("model", model);
	renderer.model.Draw(renderer.shader);
}