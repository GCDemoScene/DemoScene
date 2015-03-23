#pragma once

#include <iostream>
#include <GL/glew.h>

struct Program{
    GLuint id;

    Program(const std::string& vertPath, const std::string& geomPath, const std::string& fragPath);
    ~Program();

    GLuint getProgramId();
    
    void checkLinkError();
};
