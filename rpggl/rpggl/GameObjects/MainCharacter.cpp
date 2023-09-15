#include "MainCharacter.h"

void DebugPosition(glm::vec3 position);

void MainCharacter::Update() {
	glm::vec3 previousPos = transform.position;
	if (g_InputManager.GetKeyDown(GLFW_KEY_W))
	{
		transform.Move(previousPos + glm::vec3(0.0f, 0.0f, -1.0f));
		transform.Rotate(glm::vec3(0.0f, 0.0f, 0.0f));
		DebugPosition(transform.position);
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_S))
	{
		transform.Move(previousPos + glm::vec3(0.0f, 0.0f, 1.0f));
		transform.Rotate(glm::vec3(0.0f, 180.0f, 0.0f));
		DebugPosition(transform.position);
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_A))
	{
		transform.Move(previousPos + glm::vec3(-1.0f, 0.0f, 0.0f));
		transform.Rotate(glm::vec3(0.0f, 90.0f, 0.0f));
		DebugPosition(transform.position);
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_D))
	{
		transform.Move(previousPos + glm::vec3(1.0f, 0.0f, 0.0f));
		transform.Rotate(glm::vec3(0.0f, -90.0f, 0.0f));
		DebugPosition(transform.position);
	}
}

void DebugPosition(glm::vec3 position) 
{
	std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

