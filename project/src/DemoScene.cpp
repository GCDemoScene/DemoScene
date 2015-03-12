#include "DemoScene.hpp"
#include <iostream>
#include <chrono>
#include "Cube.hpp"
#include "Shader.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

DemoScene::DemoScene():
    FPS(60), WINDOW_WIDTH(1024), WINDOW_HEIGHT(768), FRAMERATE_MILLISECONDS(1000/FPS), glewCode(glewInit())
{
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "DemoScene : Birds around the world !");
    glewCode = glewInit();
    program.createProgram();
}

int DemoScene::initScene()
{
    if(GLEW_OK != glewCode) {
        std::cerr << "Unable to initialize GLEW : " << glewGetErrorString(glewCode) << std::endl;
        //throw std::exception("OpenGl can't be initialized correctly");
        return (EXIT_FAILURE);
    }

    // create vertex & fragment shaders
    GLuint vertShaderId = compileShaderFromFile(GL_VERTEX_SHADER, "./project/src/shaders/simple.vert");
    GLuint fragShaderId = compileShaderFromFile(GL_FRAGMENT_SHADER, "./project/src/shaders/simple.frag");

    // load programs
    program.loadProgram(vertShaderId, fragShaderId);

    // create uniforms
    diffuseLocation = glGetUniformLocation(program.id, "Diffuse");
    glProgramUniform1i(program.id, diffuseLocation, 0);
    cameraLocation = glGetUniformLocation(program.id, "Camera");
    mvpLocation = glGetUniformLocation(program.id, "MVP");

    cube.initTexture();
    cube.initBuffers();

    std::cout << "init scene" << std::endl;

    return 0;
}

void DemoScene::runScene()
{
    // Boucle de jeu avec fixed time step
    sf::Clock clock;
    sf::Time elapsedTime;
    sf::Time framerate = sf::milliseconds(FRAMERATE_MILLISECONDS);

    // Viewport
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    while (window.isOpen())
    {
        clock.restart();

        /// Events ( Handle input )
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        /// Update

        // Default states
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window.clear();

        // Get camera matrices
        glm::mat4 projection = glm::perspective(45.0f, (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.f);
        glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
        glm::mat4 objectToWorld;
        glm::mat4 mvp = projection * worldToView * objectToWorld;

        // Select shader
        glUseProgram(program.id);

        // Upload uniforms
        glProgramUniformMatrix4fv(program.id, mvpLocation, 1, 0, glm::value_ptr(mvp));
        glProgramUniform3f(program.id, cameraLocation, camera.eye.x, camera.eye.y, camera.eye.z);

        /// Draw
        cube.render();
        window.display();

        checkError("End loop");

        elapsedTime = clock.getElapsedTime();

        if(elapsedTime < framerate)
        {
            sf::sleep(framerate - elapsedTime);
        }
    }
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
