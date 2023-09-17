#include "MainCharacter.h"

void DebugPosition(glm::vec3 position);
glm::vec3 NormalizeToOneComponent(const glm::vec3& inputVec3);

void MainCharacter::Update() {
	glm::vec3 previousPos = transform.position;

	glm::vec3 front = NormalizeToOneComponent(camera->Front);
	glm::vec3 right = NormalizeToOneComponent(camera->Right);



	if (g_InputManager.GetKeyDown(GLFW_KEY_W))
	{
		transform.Move(previousPos + front);
		transform.Rotate(glm::vec3(0.0f, 0.0f, 0.0f));
		DebugPosition(transform.position);
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_S))
	{
		transform.Move(previousPos - front);
		transform.Rotate(glm::vec3(0.0f, 180.0f, 0.0f));
		DebugPosition(transform.position);
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_A))
	{
		transform.Move(previousPos + right);
		transform.Rotate(glm::vec3(0.0f, 90.0f, 0.0f));
		DebugPosition(transform.position);
	}
	if (g_InputManager.GetKeyDown(GLFW_KEY_D))
	{
		transform.Move(previousPos - right);
		transform.Rotate(glm::vec3(0.0f, -90.0f, 0.0f));
		DebugPosition(transform.position);
	}
}

void DebugPosition(glm::vec3 position)
{
	std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

glm::vec3 NormalizeToOneComponent(const glm::vec3& inputVec3)
{
	// Encuentra el índice de la componente más grande
	int maxIndex = 0;
	float maxValue = inputVec3.x;

	if (inputVec3.y > maxValue)
	{
		maxIndex = 1;
		maxValue = inputVec3.y;
	}

	if (inputVec3.z > maxValue)
	{
		maxIndex = 2;
	}

	// Crea un nuevo glm::vec3 con todas las componentes en cero
	glm::vec3 result(0.0f);

	// Establece la componente más grande en uno
	result[maxIndex] = 1.0f;

	return result;
}

