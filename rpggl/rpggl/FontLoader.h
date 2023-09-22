#pragma once
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <ft2build.h>
#include FT_FREETYPE_H  
#include "GlobalData.h"

struct FontLoader
{
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

	FontLoader() {
		SetupFontLoader();
	}

	void SetupFontLoader();
	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
};

