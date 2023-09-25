#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "camera.h"
#include "Grid.h"
#include <chrono>
#include <thread>
#include <memory>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include "GameObject.h"
#include "InputManager.h"
#include "Primitives.h"

#include "GameObjects/MainCharacter.h"
#include "GameObjects/CameraManager.h"
#include "GameObjects/LightObject.h"

#include "GlobalData.h"
#include "FontLoader.h"

float lastX = GlobalData::SCR_WIDTH / 2.0f;
float lastY = GlobalData::SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const int TICKS_PER_SECOND = 60;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 10;

bool game_is_running = true;

std::unique_ptr<MainCharacter> mainCharacter;
std::unique_ptr<FontLoader> fontLoader;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	GlobalData::camera.ProcessMouseScroll(static_cast<float>(yoffset));
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

	GLFWwindow* window = glfwCreateWindow(GlobalData::SCR_WIDTH, GlobalData::SCR_HEIGHT, "Grid engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	GlobalData::inputManager.window = window;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowUserPointer(window, &GlobalData::inputManager);
	glfwSetKeyCallback(window, InputManager::KeyCallback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	Shader lightCubeShader("Shaders/LightVertex.glsl", "Shaders/LightFragment.glsl");
	Shader simpleDepthShader("Shaders/point_shadows_depth.vs", "Shaders/point_shadows_depth.fs", "Shaders/point_shadows_depth.gs");
	Shader colorShader("Shaders/Vertex.glsl", "Shaders/Fragment.glsl" );

	Model ourModel("Visuals/SimpleCharacter/simpleCharacter.obj");
	Renderer renderer(ourModel, colorShader);
	Transform transform(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(2.5f));
	mainCharacter = std::make_unique<MainCharacter>(transform, renderer, &GlobalData::camera);

	Model planeModel("Visuals/Plane.obj");
	Renderer rendererPlane(planeModel, colorShader);
	Transform transformPlane(glm::vec3(-0.1f), glm::vec3(0.0f), glm::vec3(10.0f, 0.1f , 10.0f));
	GameObject planeGO(transformPlane, rendererPlane);

	CameraManager cameraManager(colorShader, &mainCharacter->transform);

	Grid grid(10, 10);

	std::vector<float> vertices = Primitives::GetCubeVertices();
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(1.0f, 2.0f,  1.0f),
		glm::vec3(-3.0f, 1.0f,  3.0f),
		glm::vec3(3.0f, 1.0f,  -3.0f),
		glm::vec3(-3.0f, 1.0f,  -3.0f)
	};

	unsigned int lightCubeVAO, VBO;

	Transform cameraTransform(pointLightPositions[0]);
	LightObject lightCube(cameraTransform, lightCubeShader, &colorShader, vertices, &lightCubeVAO, &VBO,0);

	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth cubemap texture
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// lighting info
	// -------------
	glm::vec3 lightPos(1.0f, 2.0f, 1.0f);

	lightCube.Awake();

	colorShader.use();
	colorShader.setInt("material.diffuse", 0);
	colorShader.setInt("numPointLights", 1);
	colorShader.setInt("depthMap", 1);

	GlobalData::camera.SetTarget(mainCharacter->transform.position, 0.0f);

	fontLoader = std::make_unique<FontLoader>();

	auto next_game_tick = std::chrono::high_resolution_clock::now();
	int loops;
	auto lastFrameTime = std::chrono::high_resolution_clock::now();

	while (game_is_running && !glfwWindowShouldClose(window))
	{
		loops = 0;

		auto currentFrameTime = std::chrono::high_resolution_clock::now();
		GlobalData::deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;

		colorShader.use();
		colorShader.setVec3("viewPos", GlobalData::camera.Position);
		colorShader.setFloat("material.shininess", 32.0f);

		// directional light
		colorShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		colorShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		colorShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		// point light 1
		lightCube.Update();

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

		// 0. create depth cubemap transformation matrices
		// -----------------------------------------------
		float near_plane = 1.0f;
		float far_plane = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		simpleDepthShader.use();
		for (unsigned int i = 0; i < 6; ++i)
			simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		simpleDepthShader.setFloat("far_plane", far_plane);
		simpleDepthShader.setVec3("lightPos", lightPos);
		glEnable(GL_CULL_FACE);
		cameraManager.Render();
		mainCharacter->Render(simpleDepthShader);
		grid.DrawGrid(simpleDepthShader);
		planeGO.Render(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, GlobalData::SCR_WIDTH, GlobalData::SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		colorShader.use();
		glEnable(GL_CULL_FACE);
		cameraManager.Render();
		mainCharacter->Render(colorShader);
		planeGO.Render(colorShader);
		grid.DrawGrid(colorShader);

		glDisable(GL_CULL_FACE);
		lightCube.Render();
		fontLoader->RenderText("Test text", 540.0f, 570.0f, 0.5f, glm::vec3(0.8f, 0.1f, 0.1f));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

