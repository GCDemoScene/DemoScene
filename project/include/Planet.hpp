#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

struct Planet{
    enum class Face{
        FRONT, BACK, LEFT, RIGHT, UP, DOWN
    };

    Planet(std::string diffusePath, std::string specularPath, glm::vec3 pos = glm::vec3(0), float radius = 1.f);
    ~Planet();

    Face face;

    int width, height;
    float radius;
    glm::vec3 position;
    std::vector<int> triangleList;
    std::vector<float> uvs;
    std::vector<float> vertices;
    std::vector<float> normals;

    // VAO & VBO
    GLuint vbo[4];
    GLuint vao;

    // Texture
    std::string pathTexture;
    GLuint diffuseTexture;
    GLuint specularTexture;

    void render(GLenum mode = GL_TRIANGLES);

    void createFace(glm::vec3 startPos, Face f, glm::vec3 translate = glm::vec3(0));

    void computeHeight(glm::vec3 &vertex);
    void mapCubeToSphere(glm::vec3 &position);
};