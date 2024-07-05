#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Mesh.h"
#include "Material.h" // Inclua o cabeçalho do Material aqui

// Protótipos das funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void moveObjectAlongPath(Mesh& object, const std::vector<glm::vec3>& controlPoints, float deltaTime);
int loadSimpleOBJ(const std::string& filepath, int& nVerts);
Material loadMaterial(const std::string& filepath);
std::vector<glm::vec3> loadControlPoints(const std::string& filepath);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Variáveis globais de controle
bool rotateX = false, rotateY = false, rotateZ = false;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05;
float pitch = 0.0, yaw = -90.0;

// Variáveis para controle do tempo
float deltaTime = 0.0f; // Tempo entre o frame atual e o anterior
float lastFrame = 0.0f; // Tempo do último frame

// Lista de pontos de controle
std::vector<glm::vec3> controlPoints1 = loadControlPoints("control_points1.txt"); // Carregar de arquivo
std::vector<glm::vec3> controlPoints2 = loadControlPoints("control_points2.txt"); // Carregar de arquivo

// Função principal
int main()
{
    // Inicialização da GLFW
    glfwInit();

    //Muita atenção aqui: alguns ambientes não aceitam essas configurações
    //Você deve adaptar para a versão do OpenGL suportada por sua placa
    //Sugestão: comente essas linhas de código para desobrir a versão e
    //depois atualize (por exemplo: 4.5 com 4 e 5)
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M6 - Curvas Parametricas", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

    //Desabilita o desenho do cursor 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD: carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Obtendo as informações de versão
    const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported " << version << std::endl;

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Carregar os coeficientes de iluminação do arquivo .mtl
    Material material1 = loadMaterial("C:\\Users\\nicko\\source\\repos\\CGCCHibrido\\3D_Models\\Cube\\cube.mtl");
    Material material2 = material1; // Copiar material1 para material2 e modificar a cor difusa para diferenciar os objetos
    material2.diffuse = glm::vec3(0.0f, 1.0f, 0.0f); // Definir uma cor diferente para o segundo objeto

    // Compilando e buildando o programa de shader
    Shader shader("Phong.vs", "Phong.fs");
    glUseProgram(shader.ID);

    // Matriz de view -- posição e orientação da câmera
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    shader.setMat4("view", glm::value_ptr(view));

    // Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    shader.setMat4("projection", glm::value_ptr(projection));

    glEnable(GL_DEPTH_TEST);

    int nVerts;
    GLuint VAO = loadSimpleOBJ("C:\\Users\\nicko\\source\\repos\\CGCCHibrido\\3D_Models\\Cube\\cube.obj", nVerts);

    Mesh object1, object2;
    object1.initialize(VAO, nVerts, &shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), material1.ambient, material1.diffuse, material1.specular, material1.shininess);
    object2.initialize(VAO, nVerts, &shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), material2.ambient, material2.diffuse, material2.specular, material2.shininess);

    // Enviar os coeficientes de iluminação para o shader
    shader.setVec3("material.ambient", material1.ambient.r, material1.ambient.g, material1.ambient.b);
    shader.setVec3("material.diffuse", material1.diffuse.r, material1.diffuse.g, material1.diffuse.b);
    shader.setVec3("material.specular", material1.specular.r, material1.specular.g, material1.specular.b);
    shader.setFloat("material.shininess", material1.shininess);

    // Definir a fonte de luz com maior intensidade
    shader.setVec3("light.position", -2.0, 10.0, 2.0);
    shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f); // Aumentando a intensidade difusa
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); // Aumentando a intensidade especular

    // Variável para controlar o tempo entre os pontos
    float moveSpeed = 1.0f;
    float accumulatedTime1 = 0.0f;
    float accumulatedTime2 = 0.0f;
    int currentPointIndex1 = 0;
    int currentPointIndex2 = 0;

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Calcula o tempo passado entre o frame atual e o último frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        // Atualizando a posição e orientação da câmera
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", glm::value_ptr(view));

        // Atualizando o shader com a posição da câmera
        shader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

        // Atualiza a posição dos objetos ao longo dos pontos de controle
        moveObjectAlongPath(object1, controlPoints1, deltaTime);
        moveObjectAlongPath(object2, controlPoints2, deltaTime);

        // Chamada de desenho - drawcall
        object1.update();
        object1.draw();
        object2.update();
        object2.draw();

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }
    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);
    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        rotateX = true;
        rotateY = false;
        rotateZ = false;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        rotateX = false;
        rotateY = true;
        rotateZ = false;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        rotateX = false;
        rotateY = false;
        rotateZ = true;
    }

    float cameraSpeed = 0.05;

    if (key == GLFW_KEY_W)
    {
        cameraPos += cameraFront * cameraSpeed;
    }
    if (key == GLFW_KEY_S)
    {
        cameraPos -= cameraFront * cameraSpeed;
    }
    if (key == GLFW_KEY_A)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (key == GLFW_KEY_D)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float offsetx = xpos - lastX;
    float offsety = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    offsetx *= sensitivity;
    offsety *= sensitivity;

    pitch += offsety;
    yaw += offsetx;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void moveObjectAlongPath(Mesh& object, const std::vector<glm::vec3>& controlPoints, float deltaTime)
{
    static int currentPointIndex = 0;
    static float accumulatedTime = 0.0f;
    const float moveSpeed = 1.0f;

    accumulatedTime += deltaTime;

    if (accumulatedTime >= moveSpeed)
    {
        accumulatedTime = 0.0f;
        currentPointIndex = (currentPointIndex + 1) % controlPoints.size();
    }

    glm::vec3 currentPos = object.getPosition();
    glm::vec3 nextPos = controlPoints[currentPointIndex];
    glm::vec3 direction = glm::normalize(nextPos - currentPos);
    float distance = glm::length(nextPos - currentPos);

    if (distance > 0.1f)
    {
        currentPos += direction * deltaTime * (distance / moveSpeed);
        object.setPosition(currentPos);
    }
}

int loadSimpleOBJ(const std::string& filepath, int& nVerts)
{
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<GLfloat> vbuffer;

    std::ifstream inputFile;
    inputFile.open(filepath.c_str());
    if (inputFile.is_open())
    {
        char line[100];
        std::string sline;

        while (!inputFile.eof())
        {
            inputFile.getline(line, 100);
            sline = line;

            std::string word;

            std::istringstream ssline(line);
            ssline >> word;

            if (word == "v")
            {
                glm::vec3 v;
                ssline >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            if (word == "vt")
            {
                glm::vec2 vt;
                ssline >> vt.s >> vt.t;
                texCoords.push_back(vt);
            }
            if (word == "vn")
            {
                glm::vec3 vn;
                ssline >> vn.x >> vn.y >> vn.z;
                normals.push_back(vn);
            }
            if (word == "f")
            {
                std::string tokens[3];
                ssline >> tokens[0] >> tokens[1] >> tokens[2];

                for (int i = 0; i < 3; i++)
                {
                    int pos = tokens[i].find("/");
                    std::string token = tokens[i].substr(0, pos);
                    int index = std::atoi(token.c_str()) - 1;
                    indices.push_back(index);

                    vbuffer.push_back(vertices[index].x);
                    vbuffer.push_back(vertices[index].y);
                    vbuffer.push_back(vertices[index].z);

                    tokens[i] = tokens[i].substr(pos + 1);
                    pos = tokens[i].find("/");
                    token = tokens[i].substr(0, pos);
                    index = std::atoi(token.c_str()) - 1;

                    vbuffer.push_back(texCoords[index].s);
                    vbuffer.push_back(texCoords[index].t);

                    tokens[i] = tokens[i].substr(pos + 1);
                    index = std::atoi(tokens[i].c_str()) - 1;

                    vbuffer.push_back(normals[index].x);
                    vbuffer.push_back(normals[index].y);
                    vbuffer.push_back(normals[index].z);
                }
            }
        }
    }
    else
    {
        std::cout << "Problema ao encontrar o arquivo " << filepath << std::endl;
    }
    inputFile.close();

    GLuint VBO, VAO;

    nVerts = vbuffer.size() / 8; // 3 posições + 2 texCoords + 3 normais

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

Material loadMaterial(const std::string& filepath)
{
    Material material;
    std::ifstream file(filepath);
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "Ka")
        {
            iss >> material.ambient.r >> material.ambient.g >> material.ambient.b;
        }
        else if (prefix == "Kd")
        {
            iss >> material.diffuse.r >> material.diffuse.g >> material.diffuse.b;
        }
        else if (prefix == "Ks")
        {
            iss >> material.specular.r >> material.specular.g >> material.specular.b;
        }
        else if (prefix == "Ns")
        {
            iss >> material.shininess;
        }
    }

    return material;
}

std::vector<glm::vec3> loadControlPoints(const std::string& filepath)
{
    std::vector<glm::vec3> points;
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open control points file: " << filepath << std::endl;
        return points;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        glm::vec3 point;
        if (!(iss >> point.x >> point.y >> point.z))
        {
            std::cerr << "Failed to parse line: " << line << std::endl;
            continue;
        }
        points.push_back(point);
    }

    file.close();
    return points;
}
