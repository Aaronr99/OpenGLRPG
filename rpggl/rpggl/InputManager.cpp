#include "InputManager.h"

// Función para detectar cuándo una tecla se presiona por primera vez
bool InputManager::GetKeyDown(int keyCode) {
	if (keyState.find(keyCode) != keyState.end() && keyState[keyCode]) {
		keyState[keyCode] = false; // Restablecer el estado de la tecla
		return true;
	}
	return false;
}

bool InputManager::GetKey(int keyCode) {
	return glfwGetKey(window, keyCode) == GLFW_PRESS;
}

// Callback para manejar el evento de teclado
void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

	if (inputManager && action == GLFW_PRESS) {
		inputManager->keyState[key] = true; // Marcar la tecla como presionada
	}
}