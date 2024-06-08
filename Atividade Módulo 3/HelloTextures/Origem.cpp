#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//stb_image
#include "stb_image.h"

// Nossa classe que armazena as infos dos shaders
#include "Shader.h"

const float Pi = 3.1419;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções - TÁ NA HORA DE REFATORAR ISSO, NÃO ACHAM? 
int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color);
int loadTexture(string path);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Função MAIN
int main()
{
    // Inicialização da GLFW
    glfwInit();

    // Muita atenção aqui: alguns ambientes não aceitam essas configurações
    // Você deve adaptar para a versão do OpenGL suportada por sua placa
    // Sugestão: comente essas linhas de código para descobrir a versão e
    // depois atualize (por exemplo: 4.5 com 4 e 5)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Essencial para computadores da Apple
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Textures!", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Obtendo as informações de versão
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Compilando e buildando o programa de shader
    Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");

    // Carregando uma textura e armazenando o identificador na memória
    GLuint texID = loadTexture("Cube.png");

    // Gerando uma geometria de quadrilátero com coordenadas de textura
    int nVerts;
    GLuint VAO = loadSimpleOBJ("../OBJ/Cube.obj", nVerts, glm::vec3(0, 0, 0));

    // Ativando o shader
    glUseProgram(shader.ID);

    // Associando com o shader o buffer de textura que conectaremos
    glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

    // Criando a matriz de projeção usando a GLM
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, -1.0f, 1.0f);

    GLint projLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glEnable(GL_DEPTH_TEST);

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();

        // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Limpa o buffer de cor
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Criando a matriz de modelo usando a GLM
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
        GLint modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Ativando o primeiro buffer de textura (0) e conectando ao identificador gerado
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        // Chamada de desenho - drawcall
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, nVerts);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }

    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);

    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

// Função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int loadTexture(string path)
{
    GLuint texID;

    // Gera o identificador da textura na memória
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Ajusta os parâmetros de wrapping e filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carregamento da imagem
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) // jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else // png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color)
{
    vector<glm::vec3> vertices;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> normals;
    vector<GLfloat> vbuffer;

    ifstream inputFile(filepath.c_str());
    if (!inputFile.is_open())
    {
        cout << "Problema ao encontrar o arquivo " << filepath << endl;
        return -1;
    }

    string line;
    while (getline(inputFile, line))
    {
        istringstream ss(line);
        string word;
        ss >> word;

        if (word == "v")
        {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (word == "vt")
        {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        }
        else if (word == "vn")
        {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (word == "f")
        {
            for (int i = 0; i < 3; ++i) // Corrigido para 3, pois estamos lidando com triângulos
            {
                string vertexData;
                ss >> vertexData;

                int vertexIndex = 0, texCoordIndex = 0, normalIndex = 0;
                sscanf_s(vertexData.c_str(), "%d/%d/%d", &vertexIndex, &texCoordIndex, &normalIndex);

                vertexIndex--; // OBJ indices are 1-based
                texCoordIndex--;
                normalIndex--;

                vbuffer.push_back(vertices[vertexIndex].x);
                vbuffer.push_back(vertices[vertexIndex].y);
                vbuffer.push_back(vertices[vertexIndex].z);

                vbuffer.push_back(color.r);
                vbuffer.push_back(color.g);
                vbuffer.push_back(color.b);

                vbuffer.push_back(texCoords[texCoordIndex].x);
                vbuffer.push_back(texCoords[texCoordIndex].y);

                vbuffer.push_back(normals[normalIndex].x);
                vbuffer.push_back(normals[normalIndex].y);
                vbuffer.push_back(normals[normalIndex].z);
            }
        }
    }
    inputFile.close();

    GLuint VBO, VAO;
    nVerts = vbuffer.size() / 11; // 3 pos + 3 cor + 2 texcoord + 3 normal

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}
