#include "DemoScene.hpp"
#include <iostream>
#include <chrono>

DemoScene::DemoScene():
    FPS(60), WINDOW_WIDTH(1024), WINDOW_HEIGHT(768), FRAMERATE_MILLISECONDS(1000/FPS)
{
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "DemoScene : Birds around the world !");
    glewCode = glewInit();
}

int DemoScene::initScene()
{
    if(GLEW_OK != glewCode) {
        std::cerr << "Unable to initialize GLEW : " << glewGetErrorString(glewCode) << std::endl;
        //throw std::exception("OpenGl can't be initialized correctly");
        return (EXIT_FAILURE);
    }
    return 0;
}

void DemoScene::runScene()
{
    // Boucle de jeu avec fixed time step
    sf::Clock clock;
    sf::Time elapsedTime;
    sf::Time framerate = sf::milliseconds(FRAMERATE_MILLISECONDS);

    while (window.isOpen())
    {
        clock.restart();

        // Events ( Handle input )
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update

        // Draw
        window.clear();
        window.display();

        elapsedTime = clock.getElapsedTime();

        if(elapsedTime < framerate)
        {
            sf::sleep(framerate - elapsedTime);
        }
    }
}

// Getters

uint DemoScene::getFPS() const
{
    return FPS;
}

uint DemoScene::getWindowWidth() const
{
    return WINDOW_WIDTH;
}

uint DemoScene::getWindowHeight() const
{
    return WINDOW_HEIGHT;
}
