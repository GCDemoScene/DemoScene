#pragma once

#include <iostream>
#include <GL/glew.h>

struct Program{
    GLuint id;

    Program();
    ~Program();
    
    void loadProgram(const std::string& vertPath, const std::string& geomPath, const std::string& fragPath);

    GLuint getProgramId();
    
    void checkLinkError();
};
