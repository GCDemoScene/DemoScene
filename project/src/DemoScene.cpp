#include "DemoScene.hpp"

#include <iostream>
#include <sstream>
#include <random>
#include <stdexcept>

#include <SFML/OpenGL.hpp>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Planet.hpp"
#include "Shader.hpp"

DemoScene::DemoScene()
    : FPS(60), WINDOW_WIDTH(1024), WINDOW_HEIGHT(768), FRAMERATE_MILLISECONDS(1000/FPS),
    window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "DemoScene : Birds around the world !", sf::Style::Default, sf::ContextSettings(32)),
    glewCode(glewInit()), state(State::PLANET)
{
    if(glewCode != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW : " << glewGetErrorString(glewCode) << std::endl;
        throw std::runtime_error("OpenGl can't be initialized correctly");
    }

    // Create all shaders programs needed for OpenGL
    programActor.loadProgram("./project/src/shaders/actor.vert", "", "./project/src/shaders/actor.frag");

    // Create usefull uniform for each program
}

DemoScene::~DemoScene()
{}

void DemoScene::initScene()
{
    // create uniforms
    diffuseLocation = glGetUniformLocation(programActor.id, "Diffuse");
    glProgramUniform1i(programActor.id, diffuseLocation, 0);
    cameraLocation = glGetUniformLocation(programActor.id, "Camera");
    mvpLocation = glGetUniformLocation(programActor.id, "MVP");
    timeLocation = glGetUniformLocation(programActor.id, "Time");
    discretizationLocation = glGetUniformLocation(programActor.id, "Discretization");
    glProgramUniform2i(programActor.id, discretizationLocation, planet.width, planet.height);

    planet.initTexture();
    planet.initBuffers();
}

void DemoScene::runScene()
{
    // event variables
    int last_x, last_y;
    bool hasClicked = false;

    // Boucle de jeu avec fixed time step
    sf::Time elapsedTime;
    sf::Time framerate = sf::milliseconds(FRAMERATE_MILLISECONDS);
    sf::Time oneSecond = sf::milliseconds(1000.f);
    int fps = 0;

    sf::Clock t;
    sf::Clock clock;
    sf::Clock elapsedASecond;

    while (running)
    {
        clock.restart();

        // Upload general uniform
        glProgramUniform1i(programActor.id, timeLocation, t.getElapsedTime().asMilliseconds());

        /// Events ( Handle input )
        event(last_x, last_y, hasClicked);

        /// Update
        update();

        /// Draw
        render();

        checkError("End loop");

        elapsedTime = clock.getElapsedTime();
        if(elapsedTime < framerate)
        {
            sf::sleep(framerate - elapsedTime);
        }

        ++fps;
        if(elapsedASecond.getElapsedTime() >= oneSecond)
        {
            std::ostringstream oss;
            oss << "DemoScene : Birds around the world !" << " (FPS : " << fps << ")";
            window.setTitle(oss.str().c_str());
            fps = 0;
            elapsedASecond.restart();
        }
    }
    window.close();
}

void DemoScene::event(int &last_x, int &last_y, bool &hasClicked)
{
    int current_x, current_y;

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            running = false;

        switch(event.type)
        {
            case sf::Event::KeyPressed :
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    running = false;

                break;

            case sf::Event::MouseButtonPressed :
                last_x = event.mouseButton.x;
                last_y = event.mouseButton.y;
                hasClicked = true;
                break;

            case sf::Event::MouseButtonReleased :
                hasClicked = false;
                break;

            case sf::Event::MouseWheelMoved :
                camera.zoom(-event.mouseWheel.delta * .2f);
                break;

            default:
                break;
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && hasClicked == true)
        {
            current_x = sf::Mouse::getPosition(window).x;
            current_y = sf::Mouse::getPosition(window).y;
            auto dX = current_x - last_x;
            auto dY = current_y - last_y;
            camera.turn(dY * .007f, dX * .007f);
            last_x = current_x;
            last_y = current_y;
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Right) && hasClicked == true)
        {
            current_x = sf::Mouse::getPosition(window).x;
            current_y = sf::Mouse::getPosition(window).y;
            auto dX = current_x - last_x;
            auto dY = current_y - last_y;
            camera.pan(dX * .0007f, dY * .0007f);
            last_x = current_x;
            last_y = current_y;
        }
    }
}

void DemoScene::update()
{
    switch (state) {
        case State::PLANET :
            std::cout << "State : Planet" << std::endl;
            changeState(State::ZOOMPLANET);
            break;

        case State::ZOOMPLANET :
            std::cout << "State : ZoomPlanet" << std::endl;
            changeState(State::BIRDS);
            break;

        case State::BIRDS :
            std::cout << "State : Birds" << std::endl;
            changeState(State::FOLLOWBIRDS);
            break;

        case State::FOLLOWBIRDS :
            std::cout << "State : FollowBirds" << std::endl;
            changeState(State::BYEBYEBIRDS);
            break;

        case State::BYEBYEBIRDS :
           // std::cout << "State : ByeByeBirds" << std::endl;
//            changeState(State::PLANET);
            break;

        default:
            break;
    }
}

void DemoScene::render()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get camera matrices
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.f);
    glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
    glm::mat4 objectToWorld;
    glm::mat4 mvp = projection * worldToView * objectToWorld;

    // Upload uniforms
    glProgramUniformMatrix4fv(programActor.id, mvpLocation, 1, 0, glm::value_ptr(mvp));
    glProgramUniform3f(programActor.id, cameraLocation, camera.eye.x, camera.eye.y, camera.eye.z);

    // Select shader
    glUseProgram(programActor.id);

    // GL Viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    planet.render();
    window.display();

    glDisable(GL_DEPTH_TEST);
}

void DemoScene::changeState(State state)
{
    this->state = state;
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

bool DemoScene::checkError(const char* title)
{
    int error;
    if((error = glGetError()) != GL_NO_ERROR)
    {
        std::string errorString;
        switch(error)
        {
        case GL_INVALID_ENUM:
            errorString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            errorString = "GL_OUT_OF_MEMORY";
            break;
        default:
            errorString = "UNKNOWN";
            break;
        }
        fprintf(stdout, "OpenGL Error(%s): %s\n", errorString.c_str(), title);
    }
    return error == GL_NO_ERROR;
}