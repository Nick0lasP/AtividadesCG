#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Mesh {
public:
    GLuint VAO;
    int nVertices;
    Shader* shader;
    glm::vec3 position;
    glm::vec3 scale;
    float angle;
    glm::vec3 axis;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    Mesh() : VAO(0), nVertices(0), shader(nullptr), position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), angle(0.0f), axis(0.0f, 1.0f, 0.0f), ambient(1.0f, 1.0f, 1.0f), diffuse(1.0f, 1.0f, 1.0f), specular(1.0f, 1.0f, 1.0f), shininess(32.0f) {}

    void initialize(GLuint VAO, int nVertices, Shader* shader, glm::vec3 position, glm::vec3 scale, float angle, glm::vec3 axis, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny);

    void draw();
    void update();
    void setPosition(const glm::vec3& pos);
    glm::vec3 getPosition() const;
};
