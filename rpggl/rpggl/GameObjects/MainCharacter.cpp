#include "MainCharacter.h"

void MainCharacter::Update() {
	glm::vec3 previousPos = transform.position;
	if (g_InputManager.GetKeyDown(GLFW_KEY_W))
	{
		transform.Move(previousPos + glm::vec3(0.0f, 0.0f, -1.0f));
		transform.Rotate(glm::vec3(0.0f, 0.0f, 0.0f));
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_S))
	{
		transform.Move(previousPos + glm::vec3(0.0f, 0.0f, 1.0f));
		transform.Rotate(glm::vec3(0.0f, 180.0f, 0.0f));
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_A))
	{
		transform.Move(previousPos + glm::vec3(-1.0f, 0.0f, 0.0f));
		transform.Rotate(glm::vec3(0.0f, 90.0f, 0.0f));
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_D))
	{
		transform.Move(previousPos + glm::vec3(1.0f, 0.0f, 0.0f));
		transform.Rotate(glm::vec3(0.0f, -90.0f, 0.0f));
	}
}

