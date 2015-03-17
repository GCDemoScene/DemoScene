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

    const uint FPS;
    const uint WINDOW_WIDTH;
    const uint WINDOW_HEIGHT;
    const uint FRAMERATE_MILLISECONDS;
    GLenum glewCode;
    sf::RenderWindow window;

    bool running;
    
    ////////////////
    //  Render
    ////////////////
    Program program;
    Camera camera;

    // Uniforms
    GLuint diffuseLocation;
    GLuint cameraLocation;
    GLuint mvpLocation;
    GLuint timeLocation;
    GLuint discretizationLocation;

    // Actors
    Planet planet;

    ///////////
    // States
    ///////////
    State state;
    void changeState(State state);
    
    // Other
    bool checkError(const char* title);
};
