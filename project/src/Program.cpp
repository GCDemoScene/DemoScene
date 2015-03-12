#include "Program.hpp"
#include <iostream>

Program::Program() :
    id(0)
{
}


Program::~Program()
{
}

void Program::createProgram()
{
    id = glCreateProgram();
    std::cout <<  "id program ctor : " << id << std::endl;
}

GLuint Program::getProgramId()
{
    if(id != 0)
    {
        return id;
    }
    else
    {
        createProgram();
        return id;
    }
}

void Program::loadProgram(GLuint vsID, GLuint fsID)
{
    glAttachShader(id, vsID);
    glAttachShader(id, fsID);
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
