#pragma once
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include "Camera.hpp"
#include "Program.hpp"
#include "Planet.hpp"

class DemoScene
{
public:
    DemoScene();
    ~DemoScene();
    int initScene();
    void runScene();

    uint getFPS() const;
    uint getWindowWidth() const;
    uint getWindowHeight() const;

private:
    enum class State
    {
        PLANET,
        ZOOMPLANET,
        BIRDS,
        FOLLOWBIRDS,
        BYEBYEBIRDS
    };

    void event(int &last_x, int &last_y, bool &hasClicked);
    void update();
    void render();
    bool checkError(const char* title);

    const uint FPS;
    const uint WINDOW_WIDTH;
    const uint WINDOW_HEIGHT;
    const uint FRAMERATE_MILLISECONDS;
    GLenum glewCode;
    sf::RenderWindow window;

    Camera camera;
    Program program;

    State state;

    void changeState(State state);

    // Uniforms
    GLuint diffuseLocation;
    GLuint cameraLocation;
    GLuint mvpLocation;

    // Actors
    Planet planet;

    bool running;
};
