#include "InputManager.h"

// Función para detectar cuándo una tecla se presiona por primera vez
bool InputManager::GetKeyDown(int keyCode) {
	if (keyState.find(keyCode) != keyState.end() && keyState[keyCode]) {
		keyState[keyCode] = false; // Restablecer el estado de la tecla
		return true;
	}
	return false;
}

// Callback para manejar el evento de teclado
void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

	if (inputManager && action == GLFW_PRESS) {
		inputManager->keyState[key] = true; // Marcar la tecla como presionada
	}
}

InputManager g_InputManager;

/*void processInput(GLFWwindow* window)
{
	if (GetKeyDown(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, true);
	}

	glm::vec3 previousPos = mainCharacter->transform.position;
	if (GetKeyDown(GLFW_KEY_W))
	{
		mainCharacter->transform.Move(previousPos + glm::vec3(0.0f, 0.0f, -1.0f));
		mainCharacter->transform.Rotate(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	if (GetKeyDown(GLFW_KEY_S))
	{
		mainCharacter->transform.Move(previousPos + glm::vec3(0.0f, 0.0f, 1.0f));
		mainCharacter->transform.Rotate(glm::vec3(0.0f, 180.0f, 0.0f));
	}

	if (GetKeyDown(GLFW_KEY_A))
	{
		mainCharacter->transform.Move(previousPos + glm::vec3(-1.0f, 0.0f, 0.0f));
		mainCharacter->transform.Rotate(glm::vec3(0.0f, 90.0f, 0.0f));
	}

	if (GetKeyDown(GLFW_KEY_D))
	{
		mainCharacter->transform.Move(previousPos + glm::vec3(1.0f, 0.0f, 0.0f));
		mainCharacter->transform.Rotate(glm::vec3(0.0f, -90.0f, 0.0f));
	}
}*/