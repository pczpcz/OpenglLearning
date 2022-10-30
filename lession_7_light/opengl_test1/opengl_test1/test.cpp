#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;
float yaw = 0.0f;
float pitch = 0.0f;

glm::vec3 cameraPos = { 0.0f, 0.0f, 5.0f };
glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
glm::vec3 cameraUp = { 0.0f, 0.1f, 0.0f };


//顶点坐标（3）， 纹理坐标（2），法线向量（3）
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
};

GLuint VAO;
GLuint VBO;
GLuint EBO;
GLuint vertexShader = 0;
GLuint fragShader = 0;
GLuint shaderProgram = 0;

GLuint lightVAO;
GLuint lightVBO;
GLuint lightVertexShader = 0;
GLuint lightFragShader = 0;
GLuint lightShaderProgram = 0;

const char* vertexShaderSource = "#version 450 core\n"
                                "layout (location = 0) in vec3 aPos;\n"
                                "layout (location = 1) in vec2 aInColorPos;\n"
                                "layout (location = 2) in vec3 aNormal;\n"
                                "out vec3 aOutNormal;\n;"
                                "out vec2 aOutColorPos;\n"
                                "out vec3 aOutFragPos;\n"
                                "uniform mat4 model;\n"
                                "uniform mat4 view;\n"
                                "uniform mat4 projection;\n"
                                "void main()\n"
                                "{\n"
                                "   aOutFragPos = vec3(model * vec4(aPos.x, aPos.y, aPos.z, 1.0));\n"
                                "   gl_Position = projection * view * vec4(aOutFragPos, 1.0);\n"     //从右到左
                                "   aOutColorPos = aInColorPos;\n"
                                "   aOutNormal = mat3(model) * aNormal;\n"           //等比缩放，不使用法线矩阵
                                "}\0";

const char* fragShaderSource = "#version 450 core\n"
                                "in vec2 aOutColorPos;\n"
                                "out vec4 FragColor;\n"
                                "in vec3 aOutNormal;\n"
                                "in vec3 aOutFragPos;\n"
                                "uniform sampler2D texture2;\n"
                                "uniform sampler2D texture1;\n"
                                "uniform vec3 viewPos;\n"
                                "struct light {\n"
                                "   vec3 position;\n"
                                "   vec3 ambientFactor;\n"
                                "   vec3 diffuseFactor;\n"
                                "   vec3 specFactor;\n"
                                "};\n"
                                "\n"
                                "struct material {\n"
                                "   vec3 ambientColor;\n"
                                "   vec3 diffuseColor;\n"
                                "   vec3 specColor;\n"
                                "   float shininess;\n"
                                "};\n"
                                "uniform light lightData;\n;"
                                "uniform material materialData;\n"
                                "void main()\n"
                                "{\n"
                                "    vec4 objColor = mix(texture(texture1, aOutColorPos), texture(texture2, aOutColorPos), 0.2);"
                                "    vec3 ambientColor3v = lightData.ambientFactor * materialData.ambientColor;\n"
                                "    vec3 norm = normalize(aOutNormal);\n"
                                "    vec3 lightDir = normalize(lightData.position - aOutFragPos);\n"
                                "    float diff = max(dot(norm, lightDir), 0.0);\n"
                                "    vec3 diffuseColor3v = lightData.diffuseFactor * (diff * materialData.diffuseColor);\n"
                                "    vec3 viewDir = normalize(viewPos - aOutFragPos);\n"
                                "    vec3 reflectDir = reflect(-lightDir, norm);\n"
                                "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialData.shininess);\n"
                                "    vec3 specularColor3v = lightData.specFactor * (spec * materialData.specColor);\n"
                                "    vec3 lightColor3v = ambientColor3v + diffuseColor3v + specularColor3v;\n"
                                "    FragColor = vec4(lightColor3v, 1.0);\n"
                                "}\0";

const char* lightVertexShaderSource = "#version 450 core\n"
                                      "layout (location = 0) in vec3 aPos;\n"
                                      "uniform mat4 model;\n"
                                      "uniform mat4 view;\n"
                                      "uniform mat4 projection;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"     //从右到左
                                      "}\0";

const char* lightFragShaderSource =   "#version 450 core\n"
                                      "out vec4 FragColor;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"     //从右到左
                                      "}\0";

int width, height, nrChannels;
unsigned char* data1 = nullptr;
unsigned char* data2 = nullptr;

GLuint texture1;
GLuint texture2;

void init()
{
    /*****************************************************物体着色器start****************************************************************/
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glNamedBufferStorage(VBO, sizeof(vertices), vertices, 0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);                             //注意是5而不是6
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(5 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    data2 = stbi_load("pathwnd.png", &width, &height, &nrChannels, 0);
    glCreateTextures(GL_TEXTURE_2D, 1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTextureStorage2D 和 glTextureSubImage2D 没有达到效果，显示全黑

    data1 = stbi_load("desktop.jpg", &width, &height, &nrChannels, 0);
    glCreateTextures(GL_TEXTURE_2D, 1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(fragShader, 1, &fragShaderSource, NULL);
    glCompileShader(fragShader);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLuint uniformPos1 = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(uniformPos1, 0);
    GLuint uniformPos2 = glGetUniformLocation(shaderProgram, "texture2");
    glUniform1i(uniformPos2, 1);
    /*****************************************************物体着色器end****************************************************************/

    /*****************************************************光源着色器start**************************************************************/
    //两个shader程序可以共用同一个VAO和VBO， 不需要单独再创建一个
    /*
    glCreateVertexArrays(1, &lightVAO);
    glCreateBuffers(1, &lightVBO);
    glNamedBufferStorage(lightVBO, sizeof(vertices), vertices, 0);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);                             //注意是5而不是6
    glEnableVertexAttribArray(0);
    */

    lightVertexShader = glCreateShader(GL_VERTEX_SHADER);
    lightFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lightVertexShader, 1, &lightVertexShaderSource, NULL);
    glCompileShader(lightVertexShader);
    glShaderSource(lightFragShader, 1, &lightFragShaderSource, NULL);
    glCompileShader(lightFragShader);
    lightShaderProgram = glCreateProgram();
    glAttachShader(lightShaderProgram, lightVertexShader);
    glAttachShader(lightShaderProgram, lightFragShader);
    glLinkProgram(lightShaderProgram);
    glUseProgram(lightShaderProgram);
    /*****************************************************光源着色器end****************************************************************/
}

void display() 
{
    glClear(GL_DEPTH_BUFFER_BIT);   //清除上一帧深度缓冲，这一帧的观察方向都不同，所以参与深度比较的数据点肯定也不同了，所以两帧的比较应该相互独立

    const float bkgColor[] = { 0.2f, 0.3f, 0.3f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, bkgColor);
    //glBindTexture(GL_TEXTURE_2D, texture1);

    /*****************************开始渲染光源*****************************************************************************************/
    glUseProgram(lightShaderProgram);

    glm::mat4 lightModel(1.0f);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f, 0.2f, 0.2f));
    lightModel = glm::rotate(lightModel, glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    lightModel = glm::translate(lightModel, glm::vec3(6.0, 0.0, 10.0));
    GLuint lightModelLoc = glGetUniformLocation(lightShaderProgram, "model");
    glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(lightModel));

    glm::mat4 lightView(1.0f);
    lightView = glm::translate(lightView, glm::vec3(0.0f, 0.0f, -5.0f));
    //lightView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint lightViewLoc = glGetUniformLocation(lightShaderProgram, "view");
    glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(lightView));

    glm::mat4 lightProjection(1.0f);
    lightProjection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 2.0f, 100.0f);            //采用透视投影， 注意远平面和近平面的设置范围
    GLuint lightProjectionLoc = glGetUniformLocation(lightShaderProgram, "projection");
    glUniformMatrix4fv(lightProjectionLoc, 1, GL_FALSE, glm::value_ptr(lightProjection));

    glBindVertexArray(VAO);                         //两个shader程序可以共用同一个VAO和VBO
    glDrawArrays(GL_TRIANGLES, 0, 36);              //注意36而不是3

    /************************************开始渲染物体**********************************************************************************/
    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);

    //glm::vec3 cameraTarget = { 0.0f, 0.0f, 0.0f };
    model = glm::rotate(model, glm::radians(25.0f), glm::vec3(1.0f, 1.0f, 1.0f));                       //旋转一下，要不然只能看到一个面
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));                                          //右手坐标系， 靠近屏幕Z为正，远离屏幕为负
    //view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(45.0f), (float)800/(float)600, 2.0f, 100.0f);            //采用透视投影， 注意远平面和近平面的设置范围                                                        //采用透视投影
    
    GLuint uniformModel = glGetUniformLocation(shaderProgram, "model");
    GLuint uniformView = glGetUniformLocation(shaderProgram, "view");
    GLuint uniformProjection = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

    GLuint uniformAmbientColor = glGetUniformLocation(shaderProgram, "materialData.ambientColor");
    glUniform3f(uniformAmbientColor, 1.0f, 0.5f, 0.31f);

    GLuint uniformDiffuseColor = glGetUniformLocation(shaderProgram, "materialData.diffuseColor");
    glUniform3f(uniformDiffuseColor, 1.0f, 0.5f, 0.31f);

    GLuint uniformSpecColor = glGetUniformLocation(shaderProgram, "materialData.specColor");
    glUniform3f(uniformSpecColor, 0.5f, 0.5f, 0.5f);

    GLuint uniformShininess = glGetUniformLocation(shaderProgram, "materialData.shininess");
    glUniform1f(uniformShininess, 64.0f);

    GLuint uniformViewPos = glGetUniformLocation(shaderProgram, "viewPos");
    glUniform3f(uniformViewPos, 0.0f, 0.0f, 10.0f);

    GLuint uniformLightPos = glGetUniformLocation(shaderProgram, "lightData.position");
    glUniform3f(uniformLightPos, 1.0f, 0.0f, 10.0f);

    GLuint uniformAmbientFactor = glGetUniformLocation(shaderProgram, "lightData.ambientFactor");
    glUniform3f(uniformAmbientFactor, 0.1f, 0.1f, 0.1f);

    GLuint uniformDiffuseFactor = glGetUniformLocation(shaderProgram, "lightData.diffuseFactor");
    glUniform3f(uniformDiffuseFactor, 0.3f, 0.3f, 0.3f);

    GLuint uniformSpecFactor = glGetUniformLocation(shaderProgram, "lightData.specFactor");
    glUniform3f(uniformSpecFactor, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(VAO);                         //两个shader程序可以共用同一个VAO和VBO
    glDrawArrays(GL_TRIANGLES, 0, 36);              //注意36而不是3
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    init();

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.005f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}