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

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

GLuint VAO;
GLuint VBO;
GLuint EBO;

GLuint vertexShader = 0;
GLuint fragShader = 0;

GLuint shaderProgram = 0;

const char* vertexShaderSource = "#version 450 core\n"
                                "layout (location = 0) in vec3 aPos;\n"
                                "layout (location = 1) in vec2 aInColorPos;\n"
                                "out vec2 aOutColorPos;\n"
                                "uniform mat4 model;\n"
                                "uniform mat4 view;\n"
                                "uniform mat4 projection;\n"
                                "void main()\n"
                                "{\n"
                                "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"     //从右到左
                                "   aOutColorPos = aInColorPos;\n"
                                "}\0";

const char* fragShaderSource = "#version 450 core\n"
                                "in vec2 aOutColorPos;\n"
                                "out vec4 FragColor;\n"
                                "uniform sampler2D texture2;\n"
                                "uniform sampler2D texture1;\n"
                                "void main()\n"
                                "{\n"
                                "    FragColor = mix(texture(texture1, aOutColorPos), texture(texture2, aOutColorPos), 0.2);\n"
                                "}\0";

int width, height, nrChannels;
unsigned char* data1 = nullptr;
unsigned char* data2 = nullptr;

GLuint texture1;
GLuint texture2;

void init() 
{
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glNamedBufferStorage(VBO, sizeof(vertices), vertices, 0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid *)0);                             //注意是5而不是6
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid *)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint uniformPos1 = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(uniformPos1, 0);

    GLuint uniformPos2 = glGetUniformLocation(shaderProgram, "texture2");
    glUniform1i(uniformPos2, 1);
}

void display() 
{
    glClear(GL_DEPTH_BUFFER_BIT);   //清除上一帧深度缓冲，这一帧的观察方向都不同，所以参与深度比较的数据点肯定也不同了，所以两帧的比较应该相互独立

    const float bkgColor[] = { 0.2f, 0.3f, 0.3f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, bkgColor);
    //glBindTexture(GL_TEXTURE_2D, texture1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);

    /*
    float radius = 10.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    glm::vec3 cameraPos = { camX, 5.0f, camZ };
    glm::vec3 cameraTarget = { 0.0f, 0.0f, 0.0f };
    glm::vec3 cameraUp = { 0.0f, 0.1f, 0.0f };
    */

    //glm::vec3 cameraTarget = { 0.0f, 0.0f, 0.0f };
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 1.0f));                       //旋转一下，要不然只能看到一个面
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));                                        //右手坐标系， 靠近屏幕Z为正，远离屏幕为负
    view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(45.0f), (float)800/(float)600, 2.0f, 100.0f);            //采用透视投影， 注意远平面和近平面的设置范围                                                        //采用透视投影
    
    GLuint uniformModel = glGetUniformLocation(shaderProgram, "model");
    GLuint uniformView = glGetUniformLocation(shaderProgram, "view");
    GLuint uniformProjection = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
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
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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