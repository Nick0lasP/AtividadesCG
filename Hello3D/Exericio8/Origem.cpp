/* Hello Cube - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 12/05/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();
void processInput(glm::vec3& position, glm::vec3& scale);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 2000, HEIGHT = 1400;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

bool rotateX = false, rotateY = false, rotateZ = false;
bool moveXPos = false, moveXNeg = false;
bool moveYPos = false, moveYNeg = false;
bool moveZPos = false, moveZNeg = false;
bool scaleUp = false, scaleDown = false;

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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D - Nickolas.", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega todos os ponteiros d funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;

    }

    // Obtendo as informações de versão
    const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);


    // Compilando e buildando o programa de shader
    GLuint shaderID = setupShader();

    // Gerando um buffer simples, com a geometria de um triângulo
    GLuint VAO = setupGeometry();

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(0.3f);

    glUseProgram(shaderID);
    glEnable(GL_DEPTH_TEST);


    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();
        processInput(position, scale);


        // Limpa o buffer de cor
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float angle = (GLfloat)glfwGetTime() * 50.0f;;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f) + position);
        model = glm::scale(model, scale);
        if (rotateX)
        {
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

        }
        else if (rotateY)
        {
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

        }
        else if (rotateZ)
        {
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

        }

        GLint modelLoc = glGetUniformLocation(shaderID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Chamada de desenho - drawcall
        // Poligono Preenchido - GL_TRIANGLES

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Desloca o modelo para desenhar o segundo cubo ao lado do primeiro
        glm::mat4 model2 = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

        // Desenha o segundo cubo
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }
    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);
    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
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

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveYPos = true;
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        moveYPos = false;

    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveYNeg = true;
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        moveYNeg = false;

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveXNeg = true;
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        moveXNeg = false;

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveXPos = true;
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        moveXPos = false;

    if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveZPos = true;
    if (key == GLFW_KEY_I && action == GLFW_RELEASE)
        moveZPos = false;

    if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveZNeg = true;
    if (key == GLFW_KEY_J && action == GLFW_RELEASE)
        moveZNeg = false;

    if (key == GLFW_KEY_U && (action == GLFW_PRESS || action == GLFW_REPEAT))
        scaleUp = true;
    if (key == GLFW_KEY_U && action == GLFW_RELEASE)
        scaleUp = false;

    if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT))
        scaleDown = true;
    if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        scaleDown = false;
}

void processInput(glm::vec3& position, glm::vec3& scale)
{
    if (moveXPos)
        position.x += 0.01f;
    if (moveXNeg)
        position.x -= 0.01f;
    if (moveYPos)
        position.y += 0.01f;
    if (moveYNeg)
        position.y -= 0.01f;
    if (moveZPos)
        position.z += 0.01f;
    if (moveZNeg)
        position.z -= 0.01f;
    if (scaleUp)
        scale *= 1.01f;
    if (scaleDown)
        scale *= 0.99f;
}

// Função para compilar e "buildar" um programa de shader simples e único neste exemplo de código
// O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
// fragmentShader source no início deste arquivo
// A função retorna o identificador do programa de shader
int setupShader()
{
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Checando erros de compilação (exibição via log no terminal)
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Checando erros de compilação (exibição via log no terminal)
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Linkando os shaders e criando o identificador do programa de shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Checando por erros de linkagem
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
    // Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
    // sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
    // Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
    // Pode ser arazenado em um VBO único ou em VBOs separados
    GLfloat vertices[] = {

        // Face traseira (vermelho)
        -0.4f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.4f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.4f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.4f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.4f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.4f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

        // Face frontal (verde)
        -0.4f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.4f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.4f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.4f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.4f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.4f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,

        // Face esquerda (azul)
        -0.4f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.4f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.4f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.4f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.4f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.4f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

        // Face direita (amarelo)
         0.4f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
         0.4f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
         0.4f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
         0.4f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
         0.4f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
         0.4f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,

         // Face inferior (ciano)
         -0.4f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
          0.4f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
          0.4f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
          0.4f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         -0.4f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         -0.4f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

         // Face superior (magenta)
         -0.4f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
          0.4f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
          0.4f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
          0.4f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
         -0.4f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
         -0.4f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,


    };

    GLuint VBO, VAO;

    //Geração do identificador do VBO
    glGenBuffers(1, &VBO);

    //Faz a conexão (vincula) do buffer como um buffer de array
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //Envia os dados do array de floats para o buffer da OpenGl
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Geração do identificador do VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);

    // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
    // e os ponteiros para os atributos 
    glBindVertexArray(VAO);

    //Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
    // Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
    // Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
    // Tipo do dado
    // Se está normalizado (entre zero e um)
    // Tamanho em bytes 
    // Deslocamento a partir do byte zero 

    //Atributo posição (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Atributo cor (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);



    // Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
    // atualmente vinculado - para que depois possamos desvincular com segurança
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
    glBindVertexArray(0);

    return VAO;
}
