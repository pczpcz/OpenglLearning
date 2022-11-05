#pragma once

#include <glad/glad.h>

#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoor;
};

enum ETextureType
{
    ETextureType_0,
    eTextureType_1,
    eTextureType_2,
    //...3
    //...4
    eTextureType_END,
};

struct Texture
{
    int id;
    std::string type;
    std::string path;
};


unsigned int TextureFromFile(const char* path, const std::string& directory);

class Mesh
{
public:
    Mesh(std::vector<Vertex>& vecVertexs, std::vector<unsigned int>& vecIndices, std::vector<Texture>& vecTextures);

    void setUpMesh();
    void Draw(GLuint shaderProgram);

private:
    std::vector<Vertex> vecVertexs;
    std::vector<unsigned int> vecIndices;
    std::vector<Texture> vecTextures;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
};

class Model
{
public:
    Model(const std::string& resourcePath);

    void Draw(GLuint shaderProgram);

private:
    void loadModel(const std::string& resourcePath);

    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

private:
    std::vector<Mesh> meshes;
    std::string resourcePath;
};