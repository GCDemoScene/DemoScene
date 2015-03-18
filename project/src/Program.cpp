#include "Program.hpp"

#include <iostream>

Program::Program() :
    id(glCreateProgram())
{
}


Program::~Program()
{
    glDeleteProgram(id);
}

GLuint Program::getProgramId()
{
    return id;
}

void Program::loadProgram(GLuint vsID, GLuint fsID, GLuint gsID)
{
    glAttachShader(id, vsID);
    glAttachShader(id, fsID);
    if(gsID)
        glAttachShader(id, gsID);

    glLinkProgram(id);
    if (checkLinkError() < 0)
        exit(1);
    std::cout << "load program" << std::endl;
}

int Program::checkLinkError()
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
        return -1;
    return 0;
}
