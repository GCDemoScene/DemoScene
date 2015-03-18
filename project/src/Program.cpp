#include "Program.hpp"

#include <iostream>
#include <stdexcept>
#include <SFML/OpenGL.hpp>


#include "Shader.hpp"

Program::Program() :
    id(glCreateProgram())
{}


Program::~Program()
{
    glDeleteProgram(id);
}

GLuint Program::getProgramId()
{
    return id;
}

void Program::loadProgram(const std::string& vertPath, const std::string& geomPath, const std::string& fragPath)
{
    if(!vertPath.empty())
    {
        GLuint vertShaderID = compileShaderFromFile(GL_VERTEX_SHADER, vertPath.c_str());
        glAttachShader(id, vertShaderID);
    }
    if(!geomPath.empty())
    {
        GLuint geomShaderID = compileShaderFromFile(GL_GEOMETRY_SHADER, geomPath.c_str());
        glAttachShader(id, geomShaderID);
    }
    if(!fragPath.empty())
    {
        GLuint fragShaderID = compileShaderFromFile(GL_FRAGMENT_SHADER, fragPath.c_str());
        glAttachShader(id, fragShaderID);
    }

    glLinkProgram(id);
    checkLinkError();
}

void Program::checkLinkError()
{
    // Get link error log size and print it eventually
    int logLength;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetProgramInfoLog(id, logLength, &logLength, log);
        fprintf(stderr, "Link : %s \n", log);
        delete[] log;
    }
    int status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
        throw std::runtime_error("Error during shader link.");
}
