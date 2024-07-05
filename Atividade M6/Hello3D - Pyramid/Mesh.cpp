#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp> // Inclua esta linha para as funções glm::translate e glm::rotate
#include <glm/gtc/type_ptr.hpp> // Inclua esta linha para glm::value_ptr

void Mesh::initialize(GLuint VAO, int nVertices, Shader* shader, glm::vec3 position, glm::vec3 scale, float angle, glm::vec3 axis, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
{
    this->VAO = VAO;
    this->nVertices = nVertices;
    this->shader = shader;
    this->position = position;
    this->scale = scale;
    this->angle = angle;
    this->axis = axis;
    this->ambient = amb;
    this->diffuse = diff;
    this->specular = spec;
    this->shininess = shiny;
}

void Mesh::update()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(angle), axis);
    model = glm::scale(model, scale);
    shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::draw()
{
    shader->Use();
    shader->setVec3("material.ambient", ambient.r, ambient.g, ambient.b);
    shader->setVec3("material.diffuse", diffuse.r, diffuse.g, diffuse.b);
    shader->setVec3("material.specular", specular.r, specular.g, specular.b);
    shader->setFloat("material.shininess", shininess);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, nVertices);
    glBindVertexArray(0);
}

void Mesh::setPosition(const glm::vec3& pos) {
    position = pos;
}

glm::vec3 Mesh::getPosition() const {
    return position;
}
