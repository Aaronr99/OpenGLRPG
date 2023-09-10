#include "Grid.h" // Incluimos el archivo de encabezado correspondiente


Grid::Grid(const int gridSizeX, const int gridSizeY) {
    sizeX = gridSizeX;
    sizeY = gridSizeY;
    // Inicializa los vértices aquí
    float tempVertices[] = {
        -0.5f, 0.0f,  0.5f,
         0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
        -0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
         0.5f, 0.0f, -0.5f
    };
    memcpy(vertices, tempVertices, sizeof(vertices));
    modelMatrices = GenerateGrid();
}

std::vector<glm::mat4> Grid::GenerateGrid() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Enlazar el VAO
    glBindVertexArray(VAO);

    // Enlazar y cargar los datos en el VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configurar los atributos de vértice
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desenlazar el VBO y el VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    int offsetX = (int)(-sizeX / 2);
    int offsetY = (int)(-sizeY / 2);
    std::vector<glm::mat4> modelMatrices;
    for (size_t i = 0; i < sizeX; i++)
    {
        for (size_t j = 0; j < sizeY; j++)
        {
            modelMatrices.push_back(glm::translate(glm::mat4(1.0f), glm::vec3((float)offsetX + i, 0.0f, (float)offsetY + j)));
        }
    }
    return modelMatrices;
}

void Grid::DrawGrid(Shader shader) {
    // Renderizar cada instancia del cuadrado
    for (const glm::mat4& model : modelMatrices)
    {
        shader.use();

        // Configurar las matrices de proyección y vista (como ya lo tienes)
        shader.setMat4("model", model);

        // Dibujar el grid (como ya lo tienes)
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 8);
    }
}





