#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

struct InputManager {
	// Definir un mapa para rastrear el estado de las teclas
	std::unordered_map<int, bool> keyState;
	GLFWwindow* window;

	InputManager() {
		window = nullptr;
	}

	bool GetKey(int keyCode);
	bool GetKeyDown(int keyCode);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
