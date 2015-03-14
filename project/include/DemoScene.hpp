#pragma once
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include "Camera.hpp"
#include "Program.hpp"
#include "Cube.hpp"

class DemoScene
{
public:
    DemoScene();
    ~DemoScene();
    int initScene();
    void runScene();
    bool checkError(const char* title);

    uint getFPS() const;
    uint getWindowWidth() const;
    uint getWindowHeight() const;

private:
    const uint FPS;
    const uint WINDOW_WIDTH;
    const uint WINDOW_HEIGHT;
    const uint FRAMERATE_MILLISECONDS;
    GLenum glewCode;
    sf::RenderWindow window;
    Camera camera;
    Program program;

    // Uniforms
    GLuint diffuseLocation;
    GLuint cameraLocation;
    GLuint mvpLocation;

    Cube cube;
};
