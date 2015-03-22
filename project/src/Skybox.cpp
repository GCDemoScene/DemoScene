#include "Skybox.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include <SFML/Graphics.hpp>
#include "simplexnoise.hpp"

Skybox::Skybox()
    : pathTexture("./project/resources/textures/skybox/")
{
    // Load texture
    sf::Image img_data;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        for(int face = 0; face < 6; ++face)
        {
            std::ostringstream path;
            path << pathTexture << faces[face] << ".jpg";
            if (!img_data.loadFromFile(path.str()))
                throw std::runtime_error("Could not load : "+ path.str());

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                        0,
                        GL_RGBA,
                        img_data.getSize().x,
                        img_data.getSize().y,
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        img_data.getPixelsPtr());
        }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Load buffer on GPU
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

Skybox::~Skybox()
{
    glDeleteTextures(1, &texture);
    glDeleteBuffers(4, vbo);
    glDeleteVertexArrays(1, &vao);
}

void Skybox::render(GLenum mode)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glBindVertexArray(vao);
        glDrawElements(mode, triangleList.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}