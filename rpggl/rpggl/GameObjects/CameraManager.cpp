#include "CameraManager.h"

void CameraManager::Update(float deltaTime)
{
	if (targetTransform->position != previousTransform.position)
	{
		GlobalData::camera.Position = targetTransform->position - offsetToTarget;
		previousTransform.position = targetTransform->position;
		GlobalData::camera.SetTarget(targetTransform->position, horizontalAngle);
	}

	float radius = 10.0f;
	float newAngle = 0.0f;

	if (GlobalData::inputManager.GetKey(GLFW_KEY_Q))
	{
		horizontalAngle += deltaTime * 200.0f;
		GlobalData::camera.OrbitAroundTarget(targetTransform->position, radius, horizontalAngle);
	}
	else if (GlobalData::inputManager.GetKey(GLFW_KEY_E))
	{
		horizontalAngle -= deltaTime * 200.0f;
		GlobalData::camera.OrbitAroundTarget(targetTransform->position, radius, horizontalAngle);
	}
}

void CameraManager::Render()
{
	glm::mat4 projection = glm::perspective(glm::radians(GlobalData::camera.Zoom), (float)GlobalData::SCR_WIDTH / (float)GlobalData::SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = GlobalData::camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
}