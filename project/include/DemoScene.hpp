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
    void initScene();
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
    
    // First create SFML window
    sf::RenderWindow window;

    // Only after we can create OpenGL context
    GLenum glewCode;

    bool running = true;
    
    ////////////////
    //  Render
    ////////////////
    Camera camera;

    Program programActor;
    Program programSkybox;
    Program programDirectionalLight;
    Program programPointLight;
    Program programSpotLight;
    Program programShadowMap;

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
