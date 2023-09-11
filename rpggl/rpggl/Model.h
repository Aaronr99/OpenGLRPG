#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 
#include "Mesh.h"
#include "Shader.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class Model
{
public:
    // Constructor
    Model(const std::string& path, bool gamma = false);

    // Dibuja el modelo
    void Draw(Shader& shader);

private:
    // Carga un modelo
    void loadModel(const std::string& path);

    // Procesa un nodo
    void processNode(aiNode* node, const aiScene* scene);

    // Procesa una malla
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // Carga las texturas de material
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;
};

#endif
