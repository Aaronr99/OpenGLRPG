#include "CameraManager.h"

void CameraManager::Update(float deltaTime)
{
	if (targetTransform->position != previousTransform.position)
	{
		camera->Position = targetTransform->position - offsetToTarget;
		previousTransform.position = targetTransform->position;
		camera->SetTarget(targetTransform->position, horizontalAngle);
	}

	// En el bucle de juego, llama a OrbitAroundTarget para que la cámara orbite alrededor del objetivo.
	float radius = 10.0f;
	//float horizontalAngle = glfwGetTime();
	float newAngle = 0.0f;

	if (g_InputManager.GetKey(GLFW_KEY_Q))
	{
		horizontalAngle += deltaTime * 200.0f;
		camera->OrbitAroundTarget(targetTransform->position, radius, horizontalAngle);
	}
	else if (g_InputManager.GetKey(GLFW_KEY_E))
	{
		horizontalAngle -= deltaTime * 200.0f;
		camera->OrbitAroundTarget(targetTransform->position, radius, horizontalAngle);
	}
}

void CameraManager::Render()
{
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
}