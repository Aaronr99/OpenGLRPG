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

float lastX = GlobalData::SCR_WIDTH / 2.0f;
float lastY = GlobalData::SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const int TICKS_PER_SECOND = 60;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 10;

bool game_is_running = true;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int fontVAO, fontVBO;
std::unique_ptr<Shader> fontShader;

std::unique_ptr<MainCharacter> mainCharacter;

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

void SetupFontLoader() 
{
	// compile and setup the shader
	// ----------------------------
	fontShader = std::make_unique<Shader>("Shaders/FontVertex.glsl", "Shaders/FontFragment.glsl");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(GlobalData::SCR_WIDTH), 0.0f, static_cast<float>(GlobalData::SCR_HEIGHT));
	fontShader->use();
	glUniformMatrix4fv(glGetUniformLocation(fontShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return ;
	}

	FT_Face face;
	if (FT_New_Face(ft, "fonts/Exo-Medium.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return ;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
	{
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		return ;
	}
	else {
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenVertexArrays(1, &fontVAO);
	glGenBuffers(1, &fontVBO);
	glBindVertexArray(fontVAO);
	glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
	// activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(fontVAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}



int main()
{
	SetupOpenGL();

	GLFWwindow* window = glfwCreateWindow(GlobalData::SCR_WIDTH, GlobalData::SCR_HEIGHT, "Unity Destroyer", NULL, NULL);
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightCubeShader("Shaders/LightVertex.glsl", "Shaders/LightFragment.glsl");
	Shader colorShader("Shaders/Vertex.glsl", "Shaders/Fragment.glsl");

	Model ourModel("Visuals/SimpleCharacter/simpleCharacter.obj");
	Renderer renderer(ourModel, colorShader);
	Transform transform(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(2.5f));
	mainCharacter = std::make_unique<MainCharacter>(transform, renderer, &GlobalData::camera);

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

	lightCube.Awake();

	colorShader.use();
	colorShader.setInt("material.diffuse", 0);
	colorShader.setInt("numPointLights", 1);

	GlobalData::camera.SetTarget(mainCharacter->transform.position, 0.0f);

	SetupFontLoader();

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

		RenderText(*fontShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		colorShader.use();

		cameraManager.Render();
		mainCharacter->Render();
		grid.DrawGrid(colorShader);

		lightCube.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

