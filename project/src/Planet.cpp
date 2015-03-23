#include "Planet.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "simplexnoise.hpp"

Planet::Planet(std::string diffusePath, std::string specularPath, glm::vec3 pos, float radius)
    : width(50), height(50), radius(radius), position(pos)
{
    assert(width > 0 && height > 0 && "Planet::Planet() : width and height must be > 0");

    // Create vertex object
    createFace(glm::vec3(-1.f, -1.f, 1.f), Face::FRONT, position); // front face
    createFace(glm::vec3(-1.f, -1.f, -1.f), Face::BACK, position); // back face
    createFace(glm::vec3(-1.f, -1.f, -1.f), Face::LEFT, position); // left face
    createFace(glm::vec3(1.f, -1.f, -1.f), Face::RIGHT, position); // right face
    createFace(glm::vec3(-1.f, 1.f, -1.f), Face::UP, position); // up face
    createFace(glm::vec3(-1.f, -1.f, -1.f), Face::DOWN, position); // down face

    //////////////////
    // Load textures
    //////////////////
    sf::Image img_data;
    pathTexture = diffusePath;
    if (!img_data.loadFromFile(pathTexture))
        throw std::runtime_error("Could not load : "+ pathTexture);

    glGenTextures(1, &diffuseTexture);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
        glTexImage2D(GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    img_data.getSize().x,
                    img_data.getSize().y,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    img_data.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    pathTexture = specularPath;
    if (!img_data.loadFromFile(pathTexture))
        throw std::runtime_error("Could not load : "+ pathTexture);

    glGenTextures(1, &specularTexture);
    glBindTexture(GL_TEXTURE_2D, specularTexture);
        glTexImage2D(GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    img_data.getSize().x,
                    img_data.getSize().y,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    img_data.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    ////////////////////////
    // Load buffer on GPU
    ////////////////////////
    glGenBuffers(4, vbo);
    glGenVertexArrays(1, &vao);

    // Bind the VAO
    glBindVertexArray(vao);

    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleList[0]) * triangleList.size(), &triangleList[0], GL_STATIC_DRAW);

    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);

    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs[0]) * uvs.size(), &uvs[0], GL_STATIC_DRAW);

    // Unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Planet::~Planet()
{
    glDeleteTextures(1, &diffuseTexture);
    glDeleteTextures(1, &specularTexture);
    glDeleteBuffers(4, vbo);
    glDeleteVertexArrays(1, &vao);
}

void Planet::render(GLenum mode)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, specularTexture);
    glBindVertexArray(vao);
        glDrawElements(mode, triangleList.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void Planet::computeHeight(glm::vec3 &vertex)
{
    // Get the direction vector from the center of the sphere
    glm::vec3 normalFromCenter = vertex;
    normalFromCenter = glm::normalize(normalFromCenter);

    // Variables for the noise
    static const float HEIGHT_MAX = .2f;
    static const float HEIGHT_MIN = 0.f;
    static const float NOISE_PERSISTENCE = 0.5f;
    static const float NOISE_OCTAVES = 1.5f;
    static const float NOISE_SCALE = 2.0f;

    // Generate the noise for the position
    float noise = scaled_octave_noise_3d(NOISE_OCTAVES, NOISE_PERSISTENCE, NOISE_SCALE, HEIGHT_MIN, HEIGHT_MAX, vertex.x, vertex.y, vertex.z);

    // Keep ocean level as base level
    if(noise <= -0.0f)
        noise = 0.0f;

    // Displace the position
    vertex += normalFromCenter * noise;
}

// For every vertex in the mesh
// Where vertices form 6 grids making a cube
// With bounds of [-1, -1, -1] to [1, 1, 1]
void Planet::mapCubeToSphere(glm::vec3& position)
{
    float x2 = position.x * position.x;
    float y2 = position.y * position.y;
    float z2 = position.z * position.z;

    position.x = position.x * sqrt(1.0f - (y2 * 0.5f) - (z2 * 0.5f) + ((y2 * z2) / 3.0f));
    position.y = position.y * sqrt(1.0f - (z2 * 0.5f) - (x2 * 0.5f) + ((z2 * x2) / 3.0f));
    position.z = position.z * sqrt(1.0f - (x2 * 0.5f) - (y2 * 0.5f) + ((x2 * y2) / 3.0f));
}

void Planet::createFace(glm::vec3 startPos, Planet::Face f, glm::vec3 translate)
{
    glm::vec3 pos, normal;

    for (int y = 0; y <= height; ++y) {
        for (int x = 0; x <= width; ++x) {
            pos = startPos;

            if(f == Face::FRONT || f == Face::BACK)
            {
                pos.x += (float)x / (float)width * 2.f;
                pos.y += (float)y / (float)height * 2.f;
            }
            else if(f == Face::UP || f == Face::DOWN)
            {
                pos.x += (float)x / (float)width * 2.f;
                pos.z += (float)y / (float)height * 2.f;
            }
            else if(f == Face::RIGHT || f == Face::LEFT)
            {
                pos.z += (float)x / (float)width * 2.f;
                pos.y += (float)y / (float)height * 2.f;
            }

            mapCubeToSphere(pos);
            computeHeight(pos);

            normal = glm::normalize(pos - glm::vec3(0));

            pos *= radius;
            pos += translate;

            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);

            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }

    for(int y = height; y >= 0; --y) {
        for(int x = 0; x <= width; ++x) {
            uvs.push_back(x / (float)width);
            uvs.push_back(y / (float)height);
        }
    }


    // for each quad of each face
    auto offset = (int)f * (height + 1) * (width + 1);
//    std::cout << "face n°" << (int)f << std::endl;
//    std::cout << "offset : " << offset << std::endl;
    for (int y = 0; y < height; ++y) {
        for (int x = 0 + offset; x < width + offset; ++x) {
            // triangle 1
            triangleList.push_back(x + y * (width + 1));
            triangleList.push_back(x + 1 + y * (width + 1));
            triangleList.push_back(x + (y + 1) * (width + 1));

            //triangle 2
            triangleList.push_back(x + (y + 1) * (width + 1));
            triangleList.push_back(x + 1 + y * (width + 1));
            triangleList.push_back(x + 1 + (y + 1) * (width + 1));
        }
    }
}