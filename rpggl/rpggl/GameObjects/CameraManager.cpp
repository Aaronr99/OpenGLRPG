#include "CameraManager.h"

void CameraManager::Update() 
{
	if (targetTransform->position != previousTransform.position) 
	{
		camera->Position = targetTransform->position - offsetToTarget;
		previousTransform.position = targetTransform->position;
	}
}

void CameraManager::Render()
{
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
}