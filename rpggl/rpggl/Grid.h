#pragma once // Directiva para evitar inclusión múltiple

#include <glad/glad.h>
#include "Shader.h"
#include <vector>
#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>

struct Grid {
    Grid(const int gridSizeX, const int gridSizeY);
    void DrawGrid(Shader shader);
    int sizeX, sizeY;
    unsigned int VAO, VBO;
    float vertices[24];
    std::vector<glm::mat4> modelMatrices;

private:
    std::vector<glm::mat4> GenerateGrid();
};
