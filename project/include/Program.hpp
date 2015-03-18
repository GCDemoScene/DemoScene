#pragma once
#include <GL/glew.h>

struct Program{
    GLuint id;

    Program();
    ~Program();
    
    void loadProgram(GLuint vsID, GLuint fsID, GLuint gsID = 0);
    int checkLinkError();
    
    GLuint getProgramId();
};
