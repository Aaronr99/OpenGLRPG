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
#include "InputManager.h"
#include "Primitives.h"

#include "GameObjects/MainCharacter.h"
#include "GameObjects/CameraManager.h"


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

std::unique_ptr<MainCharacter> mainCharacter;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Unity Destroyer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowUserPointer(window, &g_InputManager);
	glfwSetKeyCallback(window, InputManager::KeyCallback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	Shader lightCubeShader("Shaders/LightVertex.glsl", "Shaders/LightFragment.glsl");
	Shader colorShader("Shaders/Vertex.glsl", "Shaders/Fragment.glsl");
	Model ourModel("Visuals/SimpleCharacter/simpleCharacter.obj");
	Renderer renderer(ourModel, colorShader);
	Transform transform(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(2.5f));
	mainCharacter = std::make_unique<MainCharacter>(transform, renderer);

	CameraManager cameraManager(colorShader, camera, SCR_WIDTH, SCR_HEIGHT, &mainCharacter->transform);

	Grid grid(10, 10);

	std::vector<float> vertices = Primitives::GetCubeVertices();
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(5.0f, 1.0f,  2.0f),
		glm::vec3(-5.0f, 1.0f,  2.0f),
		glm::vec3(5.0f, 1.0f,  -2.0f),
		glm::vec3(-5.0f, 1.0f,  -2.0f)
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	colorShader.use();
	colorShader.setInt("material.diffuse", 0);
	colorShader.setInt("numPointLights", 4);

	auto next_game_tick = std::chrono::high_resolution_clock::now();
	int loops;
	auto lastFrameTime = std::chrono::high_resolution_clock::now();

	while (game_is_running && !glfwWindowShouldClose(window))
	{
		loops = 0;

		auto currentFrameTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;

		colorShader.use();
		colorShader.setVec3("viewPos", camera.Position);
		colorShader.setFloat("material.shininess", 32.0f);

		// directional light
		colorShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		colorShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		colorShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		// point light 1
		colorShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		colorShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		colorShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		colorShader.setFloat("pointLights[0].constant", 1.0f);
		colorShader.setFloat("pointLights[0].linear", 0.09f);
		colorShader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		colorShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		colorShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		colorShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		colorShader.setFloat("pointLights[1].constant", 1.0f);
		colorShader.setFloat("pointLights[1].linear", 0.09f);
		colorShader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		colorShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		colorShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		colorShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		colorShader.setFloat("pointLights[2].constant", 1.0f);
		colorShader.setFloat("pointLights[2].linear", 0.09f);
		colorShader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		colorShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		colorShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		colorShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		colorShader.setFloat("pointLights[3].constant", 1.0f);
		colorShader.setFloat("pointLights[3].linear", 0.09f);
		colorShader.setFloat("pointLights[3].quadratic", 0.032f);

		// Update
		while (std::chrono::high_resolution_clock::now() > next_game_tick && loops < MAX_FRAMESKIP) {
			mainCharacter->Update();
			cameraManager.Update();
			next_game_tick += std::chrono::milliseconds(SKIP_TICKS);
			loops++;
		}

		// Render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		colorShader.use();

		cameraManager.Render();
		mainCharacter->Render();
		grid.DrawGrid(colorShader);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.5f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

