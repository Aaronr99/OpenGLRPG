#pragma once

#include <glad/glad.h>
#include "Shader.h"
#include <vector>
#include <glm/ext/matrix_transform.hpp>

struct Grid {
    Grid(const int gridSizeX, const int gridSizeY);
    int sizeX, sizeY;
    unsigned int VAO, VBO;
    float vertices[24];
    std::vector<glm::mat4> modelMatrices;
    void DrawGrid(Shader shader);
    std::vector<glm::mat4> GenerateGrid();
};
