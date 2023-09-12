#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "camera.h"
#include "Grid.h"
#include <chrono>
#include <thread>
#include <memory>
#include <unordered_map>

#include "GameObject.h"


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 5.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const int TICKS_PER_SECOND = 60;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 10;
float deltaTime;

bool game_is_running = true;

std::unique_ptr<GameObject> mainCharacter;

// Definir un mapa para rastrear el estado de las teclas
std::unordered_map<int, bool> keyState;

// Función para detectar cuándo una tecla se presiona por primera vez
bool GetKeyDown(int keyCode)
{
	if (keyState.find(keyCode) != keyState.end() && keyState[keyCode])
	{
		keyState[keyCode] = false; // Restablecer el estado de la tecla
		return true;
	}
	return false;
}

// Callback para manejar el evento de teclado
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		keyState[key] = true; // Marcar la tecla como presionada
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{	
	if (GetKeyDown(GLFW_KEY_ESCAPE)) 
	{
		glfwSetWindowShouldClose(window, true);
	}

	glm::vec3 previousPos = mainCharacter->transform.position;
	if (GetKeyDown(GLFW_KEY_W))
	{
		mainCharacter->transform.Move(previousPos + glm::vec3(0.0f, 0.0f, -1.0f));
	}

	if (GetKeyDown(GLFW_KEY_S))
	{
		mainCharacter->transform.Move(previousPos + glm::vec3(0.0f, 0.0f, 1.0f));
	}

	if (GetKeyDown(GLFW_KEY_A))
	{
		mainCharacter->transform.Move(previousPos + glm::vec3(-1.0f, 0.0f, 0.0f));
	}

	if (GetKeyDown(GLFW_KEY_D))
	{
		mainCharacter->transform.Move(previousPos + glm::vec3(1.0f, 0.0f, 0.0f));
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

/*unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}*/

void SetupOpenGL()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int main()
{
	SetupOpenGL();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader colorShader("Shaders/Vertex.glsl", "Shaders/Fragment.glsl");
	Model ourModel("Visuals/SimpleCharacter/simpleCharacter.obj");
	Renderer renderer(ourModel, colorShader);
	Transform transform(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(2.5f));
	mainCharacter = std::make_unique<GameObject>(transform, renderer); 

	Grid grid(10, 10);

	auto next_game_tick = std::chrono::high_resolution_clock::now();
	int loops;
	auto lastFrameTime = std::chrono::high_resolution_clock::now();

	while (game_is_running && !glfwWindowShouldClose(window))
	{
		loops = 0;

		auto currentFrameTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;

		while (std::chrono::high_resolution_clock::now() > next_game_tick && loops < MAX_FRAMESKIP) {
			//Update Game
			processInput(window);
			next_game_tick += std::chrono::milliseconds(SKIP_TICKS);
			loops++;
		}

		//Render Game
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		colorShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		colorShader.setMat4("projection", projection);
		colorShader.setMat4("view", view);

		mainCharacter->Render();
		grid.DrawGrid(colorShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

