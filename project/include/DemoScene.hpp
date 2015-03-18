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
    
    bool running;
    
    // First create SFML window
    sf::RenderWindow window;

    // Only after we can create OpenGL context
    GLenum glewCode;

    ///////////
    // States
    ///////////
    State state;
    void changeState(State state);
    
    ////////////////
    //  Render
    ////////////////
    // Actors (Actors must be created before shaders programs. Program can be refered some Actors attributes)
    Planet planet;
    Camera camera;
    
    // Programs shaders
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
    
    // Other
    bool checkError(const char* title);
};
