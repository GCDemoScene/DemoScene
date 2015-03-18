#pragma once
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <vector>

struct Planet{
    enum class Face{
        FRONT, BACK, LEFT, RIGHT, UP, DOWN
    };

    Planet();
    ~Planet();

    Face face;

    int width, height;
    float radius;
    std::vector<int> triangleList;
    std::vector<float> uvs;
    std::vector<float> vertices;
    std::vector<float> normals;

    // VAO & VBO
    GLuint vbo[4];
    GLuint vao;

    // Texture
    std::string pathTexture;
    GLuint texture;

    void bind();
    void unbind();
    void render();
    void initTexture();
    void initBuffers();

    void computeHeight(glm::vec3 &vertex);
    void mapCubeToSphere(glm::vec3 &position);

    void createFace(glm::vec3 startPos, Face f);
};

