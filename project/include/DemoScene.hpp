#pragma once
#include <SFML/Graphics.hpp>
#include <GL/glew.h>

class DemoScene
{
public:
    DemoScene();
    int initScene();
    void runScene();

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
};
