#include "model.h"

unsigned int TextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
}

Mesh::Mesh(std::vector<Vertex>& vecVertexs, std::vector<unsigned int>& vecIndices, std::vector<Texture>& vecTextures)
{
    this->vecVertexs = vecVertexs;
    this->vecIndices = vecIndices;
    this->vecTextures = vecTextures;
}

void Mesh::setUpMesh()
{
    GLenum error = glGetError();
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vecVertexs.size() * sizeof(Vertex), &vecVertexs[0], GL_STATIC_DRAW);

    error = glGetError();

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);         //调用glBufferData前要先绑定
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(unsigned int), &vecIndices[0], GL_STATIC_DRAW);

    error = glGetError();

    // 顶点位置

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
    error = glGetError();
    // 顶点法线
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
    error = glGetError();
    // 顶点纹理坐标
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
    error = glGetError();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    error = glGetError();
}

void Mesh::Draw(GLuint shaderProgram)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < vecTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
        // 获取纹理序号（diffuse_textureN 中的 N）
        std::string number;
        std::string name = vecTextures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        GLuint uniformTexture = glGetUniformLocation(shaderProgram, (/*"material." + */name + number).c_str());
        glUniform1i(uniformTexture, 1);

        glBindTexture(GL_TEXTURE_2D, vecTextures[i].id);
    }

    // 绘制网格
    GLenum error = glGetError();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(vecIndices.size()), GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, vecVertexs.size(), 0);
    glBindVertexArray(0);
    error = glGetError();

    glActiveTexture(GL_TEXTURE0);
}

Model::Model(const std::string& resourcePath)
{
    loadModel(resourcePath);
    for (int i = 0; i < meshes.size(); ++i) 
    {
        meshes[i].setUpMesh();
    }
}

// draws the model, and thus all its meshes
void Model::Draw(GLuint shaderProgram)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shaderProgram);
}

void Model::loadModel(const std::string& resourcePath)
{
    Assimp::Importer import;
    this->resourcePath = resourcePath.substr(0, resourcePath.find_last_of('/'));
    const aiScene* scene = import.ReadFile(resourcePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    //处理顶点
    std::vector<Vertex> vecVertexs;
    std::vector<unsigned int> vecIndices;
    std::vector<Texture> vecTextures;
    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position = { mesh->mVertices[i].x,  mesh->mVertices[i].y , mesh->mVertices[i].z };

        if (mesh->HasNormals())
        {
            vertex.normal = { mesh->mNormals[i].x,  mesh->mNormals[i].y , mesh->mNormals[i].z };
        }

        if (mesh->mTextureCoords[0])
        {
            //一个顶点可以有8个纹理坐标
            vertex.textureCoor = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }

        vecVertexs.push_back(vertex);
    }

    //处理索引
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
        {
            vecIndices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    //处理材质
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    vecTextures.insert(vecTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    vecTextures.insert(vecTextures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    vecTextures.insert(vecTextures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    vecTextures.insert(vecTextures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vecVertexs, vecIndices, vecTextures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), this->resourcePath);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}