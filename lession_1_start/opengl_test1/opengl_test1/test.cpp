#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/*
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};*/

float vertices[] = {
    0.5f, 0.5f, 0.0f,   // ���Ͻ�
    0.5f, -0.5f, 0.0f,  // ���½�
    -0.5f, -0.5f, 0.0f, // ���½�
    -0.5f, 0.5f, 0.0f   // ���Ͻ�
};

unsigned int indices[] = {
    // ע��������0��ʼ! 
    // ����������(0,1,2,3)���Ƕ�������vertices���±꣬
    // �����������±��������ϳɾ���

    0, 1, 3, // ��һ��������
    1, 2, 3  // �ڶ���������
};

GLuint VAO;
GLuint VBO;
GLuint EBO;

GLuint vertexShader = 0;
GLuint fragShader = 0;

GLuint shaderProgram = 0;

const char* vertexShaderSource = "#version 450 core\n"
                                "layout (location = 0) in vec3 aPos;\n"
                                "void main()\n"
                                "{\n"
                                "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                "}\0";

const char* fragShaderSource = "#version 450 core\n"
                                "layout (location = 0) out vec4 FragColor;\n"
                                "void main()\n"
                                "{\n"
                                "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                "}\0";

void init() 
{
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glNamedBufferStorage(VBO, sizeof(vertices), vertices, 0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glCreateBuffers(1, &EBO);
    glNamedBufferStorage(EBO, sizeof(indices), indices, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid *)0);
    glEnableVertexAttribArray(0);
}

void display() 
{
    const float black[] = { 0.2f, 0.3f, 0.3f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, black);
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    init();

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