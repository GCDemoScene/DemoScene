#pragma once
#include <GL/glew.h>

struct Program{

    GLuint id;

    Program();
    ~Program();
    void loadProgram(GLuint vsID, GLuint fsID);
    int checkLinkError();
    void createProgram();
    GLuint getProgramId();
};
