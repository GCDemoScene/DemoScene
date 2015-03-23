#include "DemoScene.hpp"

#include <iostream>
#include <sstream>
#include <random>
#include <stdexcept>

#include <SFML/OpenGL.hpp>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"

#include "Shader.hpp"
#include "lights/PointLight.hpp"
#include "lights/DirectionalLight.hpp"
#include "lights/SpotLight.hpp"
#include "Planet.hpp"

DemoScene::DemoScene()
    : FPS(60), WINDOW_WIDTH(1024), WINDOW_HEIGHT(768), FRAMERATE_MILLISECONDS(1000/FPS), running(true),
    window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "DemoScene : Birds around the world !", sf::Style::Default, sf::ContextSettings(32)),
    glewCode(glewInit()), state(State::PLANET),
    planet("./project/resources/textures/earthTexDiffuse.png", "./project/resources/textures/earthTexSpecular.png"),
    satellite("./project/resources/textures/lavaTexDiffuse.png", "./project/resources/textures/lavaTexSpecular.png", glm::vec3(-4.f, 0.f, 0.f), .5f),
    programPlanet(Program("./project/src/shaders/planet.vert", "", "./project/src/shaders/planet.frag")),
    programPointLight(Program("./project/src/shaders/pointLight.vert", "", "./project/src/shaders/pointLight.frag")),
    programDirectionalLight(Program("./project/src/shaders/directionalLight.vert", "", "./project/src/shaders/directionalLight.frag")),
    programSpotLight(Program("./project/src/shaders/spotLight.vert", "", "./project/src/shaders/spotLight.frag")),
    programShadowMap(Program("./project/src/shaders/shadowMap.vert", "", "./project/src/shaders/shadowMap.frag")),
    programShadowCubeMap(Program("./project/src/shaders/shadowCubeMap.vert", "./project/src/shaders/shadowCubeMap.geom", "./project/src/shaders/shadowCubeMap.frag")),
    programSkybox(Program("./project/src/shaders/skybox.vert", "", "./project/src/shaders/skybox.frag"))
{
    if(glewCode != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW : " << glewGetErrorString(glewCode) << std::endl;
        throw std::runtime_error("OpenGl can't be initialized correctly");
    }

    // Texture color
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Texture normal
    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Texture depth
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create shadow texture
    glGenTextures(shadowMapCount, shadowMapTextures);
    for (int i = 0; i < shadowMapCount; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, shadowMapTextures[i]);
        // Bilinear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Color needs to be 0 outside of texture coordinates
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // Params to shadowMap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        // Create empty texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }

    glGenTextures(shadowCubeMapCount, shadowCubeMapTextures);
    for (int i = 0; i < shadowCubeMapCount; ++i)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMapTextures[i]);
        // Bilinear filtering
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // Params to shadowMap
        // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        for (int face = 0; face < 6; ++face)
            // Create empty texture faces
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT24, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }

    // ---------------
    // QuadScreen
    // ---------------
    glGenVertexArrays(1, &quadVao);
    glGenBuffers(2, quadVbo);
    // Bind the VAO
    glBindVertexArray(quadVao);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadVbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_triangleList), quad_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    // ----------------
    // SSBO - Lights 
    // ----------------
    // Generate Shader Storage Objects
    glGenBuffers(3, ssbo);

    // ---------------------------
    // Create Framebuffer Object
    // ---------------------------
    glGenFramebuffers(1, &gbufferFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
        // Initialize DrawBuffers
        gbufferDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
        gbufferDrawBuffers[1] = GL_COLOR_ATTACHMENT1;
        glDrawBuffers(2, gbufferDrawBuffers);
        // Attach textures to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, colorTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 , GL_TEXTURE_2D, normalTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("OpenGl : Error on building framebuffer");

    // -------------------
    // Create shadow FBO
    // -------------------
    glGenFramebuffers(1, &shadowFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
        // Create a render buffer since we don't need to read shadow color
        // in a texture
        glGenRenderbuffers(1, &shadowRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, shadowMapSize, shadowMapSize);
        // Attach the renderbuffer
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadowRenderBuffer);
        // Attach the first shadow texture to the depth attachment
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, shadowCubeMapTextures[0], 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("OpenGl : Error on building shadow framebuffer");
    // Back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create usefull uniform
    // Planet
        timeLocation = glGetUniformLocation(programPlanet.id, "Time");
        mvpLocation = glGetUniformLocation(programPlanet.id, "MVP");
        diffuseLocation = glGetUniformLocation(programPlanet.id, "Diffuse");
        specularLocation = glGetUniformLocation(programPlanet.id, "Specular");
        specularPowerLocation = glGetUniformLocation(programPlanet.id, "SpecularPower");
        discretizationLocation = glGetUniformLocation(programPlanet.id, "Discretization");
        radiusLocation = glGetUniformLocation(programPlanet.id, "Radius");
        positionLocation = glGetUniformLocation(programPlanet.id, "PlanetPosition");
        
        glProgramUniform1i(programPlanet.id, diffuseLocation, 0);
        glProgramUniform1i(programPlanet.id, specularLocation, 1);
        glProgramUniform1f(programPlanet.id, specularPowerLocation, 25);

    // PointLight
        pl_colorBufferLocation = glGetUniformLocation(programPointLight.id, "ColorBuffer");
        pl_normalBufferLocation = glGetUniformLocation(programPointLight.id, "NormalBuffer");
        pl_depthBufferLocation = glGetUniformLocation(programPointLight.id, "DepthBuffer");
        pl_shadowCubeMapLocation = glGetUniformLocation(programPointLight.id, "ShadowCubeMap");
        pl_IdLocation = glGetUniformLocation(programPointLight.id, "Id");
        pl_cameraPositionLocation = glGetUniformLocation(programPointLight.id, "CameraPosition");
        pl_inverseProjLocation = glGetUniformLocation(programPointLight.id, "InverseProj");
        pl_mvpLocation = glGetUniformLocation(programPointLight.id, "MVP");

        glProgramUniform1i(programPointLight.id, pl_colorBufferLocation, 0);
        glProgramUniform1i(programPointLight.id, pl_normalBufferLocation, 1);
        glProgramUniform1i(programPointLight.id, pl_depthBufferLocation, 2);

     // DirectionalLight
        dl_colorBufferLocation = glGetUniformLocation(programDirectionalLight.id, "ColorBuffer");
        dl_normalBufferLocation = glGetUniformLocation(programDirectionalLight.id, "NormalBuffer");
        dl_depthBufferLocation = glGetUniformLocation(programDirectionalLight.id, "DepthBuffer");
        dl_shadowMapLocation = glGetUniformLocation(programDirectionalLight.id, "ShadowMap");
        dl_IdLocation = glGetUniformLocation(programDirectionalLight.id, "Id");
        dl_cameraPositionLocation = glGetUniformLocation(programDirectionalLight.id, "CameraPosition");
        dl_inverseProjLocation = glGetUniformLocation(programDirectionalLight.id, "InverseProj");

        glProgramUniform1i(programDirectionalLight.id, dl_colorBufferLocation, 0);
        glProgramUniform1i(programDirectionalLight.id, dl_normalBufferLocation, 1);
        glProgramUniform1i(programDirectionalLight.id, dl_depthBufferLocation, 2);

    // SpotLight
        sl_colorBufferLocation = glGetUniformLocation(programSpotLight.id, "ColorBuffer");
        sl_normalBufferLocation = glGetUniformLocation(programSpotLight.id, "NormalBuffer");
        sl_depthBufferLocation = glGetUniformLocation(programSpotLight.id, "DepthBuffer");
        sl_shadowMapLocation = glGetUniformLocation(programSpotLight.id, "ShadowMap");
        sl_IdLocation = glGetUniformLocation(programSpotLight.id, "Id");
        sl_cameraPositionLocation = glGetUniformLocation(programSpotLight.id, "CameraPosition");
        sl_inverseProjLocation = glGetUniformLocation(programSpotLight.id, "InverseProj");

        glProgramUniform1i(programSpotLight.id, sl_colorBufferLocation, 0);
        glProgramUniform1i(programSpotLight.id, sl_normalBufferLocation, 1);
        glProgramUniform1i(programSpotLight.id, sl_depthBufferLocation, 2);

    // ShadowMap
        objectToLightScreenLocation = glGetUniformLocation(programShadowMap.id, "ObjectToLightScreen");
    
    // ShadowCubeMap
        objectToLightScreen2Location = glGetUniformLocation(programShadowCubeMap.id, "ObjectToLightScreen");

    // Skybox
        skyboxCubeMapLocation = glGetUniformLocation(programSkybox.id, "CubeMap");
        skyboxMVPLocation = glGetUniformLocation(programSkybox.id, "MVP");
        skyboxTimeLocation = glGetUniformLocation(programSkybox.id, "Time");
        
        glProgramUniform1i(programSkybox.id, skyboxCubeMapLocation, 0);
}

DemoScene::~DemoScene()
{
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteTextures(shadowMapCount, shadowMapTextures);
    glDeleteTextures(shadowCubeMapCount, shadowCubeMapTextures);

    glDeleteVertexArrays(1, &quadVao);
    glDeleteBuffers(4, quadVbo);
    glDeleteBuffers(3, ssbo);
    glDeleteFramebuffers(1, &gbufferFbo);
    glDeleteRenderbuffers(1, &shadowRenderBuffer);
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

        auto time = t.getElapsedTime().asMilliseconds();

        // Upload general uniform
        glProgramUniform1i(programPlanet.id, timeLocation, time);
        glProgramUniform1i(programSkybox.id, skyboxTimeLocation, time);

        /// Events ( Handle input )
        event(last_x, last_y, hasClicked);

        /// Update
        update();

        /// Draw
        render(t.getElapsedTime().asMilliseconds());

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

void DemoScene::render(float deltaTime)
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get camera matrices
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.f);
    glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
    glm::mat4 objectToWorld;
    glm::mat4 mvp = projection * worldToView * objectToWorld;
    // Compute the inverse worldToScreen matrix
    glm::mat4 inverseProj = glm::inverse(mvp);

    // Upload uniforms
    // Send transformations
    glProgramUniformMatrix4fv(programPlanet.id, mvpLocation, 1, 0, glm::value_ptr(mvp));
    glProgramUniformMatrix4fv(programSkybox.id, skyboxMVPLocation, 1, 0, glm::value_ptr(mvp));
    glProgramUniformMatrix4fv(programPointLight.id, pl_mvpLocation, 1, 0, glm::value_ptr(mvp));
    
    // Send camera position
    glProgramUniform3f(programPointLight.id, pl_cameraPositionLocation, camera.eye.x, camera.eye.y, camera.eye.z);
    glProgramUniform3f(programDirectionalLight.id, dl_cameraPositionLocation, camera.eye.x, camera.eye.y, camera.eye.z);
    glProgramUniform3f(programSpotLight.id, sl_cameraPositionLocation, camera.eye.x, camera.eye.y, camera.eye.z);

    // screen to world space
    glProgramUniformMatrix4fv(programPointLight.id, pl_inverseProjLocation, 1, 0, glm::value_ptr(inverseProj));
    glProgramUniformMatrix4fv(programDirectionalLight.id, dl_inverseProjLocation, 1, 0, glm::value_ptr(inverseProj));
    glProgramUniformMatrix4fv(programSpotLight.id, sl_inverseProjLocation, 1, 0, glm::value_ptr(inverseProj));

    //////////////////////////////////////////////////
    // Buffer Storage : Lights & Renders ShadowMaps //
    //////////////////////////////////////////////////
    int pointLightBufferSize = 4 * sizeof(int) + sizeof(PointLight) * nbBirds;
    int directionalLightBufferSize = 4 * sizeof(int) + sizeof(DirectionalLight) * nbSun;
    int spotLightBufferSize = 4 * sizeof(int) + sizeof(SpotLight) * nbRobots;

    glEnable(GL_DEPTH_TEST);

    // Bind the shadow FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);

    // Set the viewport corresponding to shadow texture resolution
    glViewport(0, 0, shadowMapSize, shadowMapSize);

    // Clear only the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    void * lightBuffer = NULL;

    glUseProgram(programShadowCubeMap.id);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pointLightBufferSize, 0, GL_DYNAMIC_COPY);
    lightBuffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    ((int*) lightBuffer)[0] = nbBirds;
    for (int i = 0; i < nbBirds; ++i)
    {
        glm::vec3 pos(cos(deltaTime * 0.0002 + 0.5f * i), 0.75f, sin(deltaTime * 0.0002 + 0.5f * i));
        glm::vec3 color(fabsf(cos(i*2.f)), 1.-fabsf(sinf(i)) , 0.5f + 0.5f-fabsf(cosf(i)));
        // Light space matrices
        // From light space to shadow map screen space
        glm::mat4 projectionLight = glm::perspective(glm::radians(90.f), 1.f, 1.f, 100.f);
        // From world to light
        glm::mat4 worldToLight;
        glm::mat4 objectToWorld;
        // From object to light
        glm::mat4 objectToLight;
        // From object to shadow map screen space
        glm::mat4 objectToLightScreen[6];
        // From world to shadow map screen space
        glm::mat4 worldToLightScreen[6];
        // Edge X
        worldToLight = glm::lookAt(pos, pos + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));
        objectToLight = worldToLight * objectToWorld;
        objectToLightScreen[0] = projectionLight * objectToLight;
        worldToLightScreen[0] = projectionLight * worldToLight;
        // Edge - X
        worldToLight = glm::lookAt(pos, pos + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));
        objectToLight = worldToLight * objectToWorld;
        objectToLightScreen[1] = projectionLight * objectToLight;
        worldToLightScreen[1] = projectionLight * worldToLight;
        // Edge Y
        worldToLight = glm::lookAt(pos, pos + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
        objectToLight = worldToLight * objectToWorld;
        objectToLightScreen[2] = projectionLight * objectToLight;
        worldToLightScreen[2] = projectionLight * worldToLight;
        // Edge - Y
        worldToLight = glm::lookAt(pos, pos + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
        objectToLight = worldToLight * objectToWorld;
        objectToLightScreen[3] = projectionLight * objectToLight;
        worldToLightScreen[3] = projectionLight * worldToLight;
        // Edge Z
        worldToLight = glm::lookAt(pos, pos + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f));
        objectToLight = worldToLight * objectToWorld;
        objectToLightScreen[4] = projectionLight * objectToLight;
        worldToLightScreen[4] = projectionLight * worldToLight;
        // Edge - Z
        worldToLight = glm::lookAt(pos, pos + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f));
        objectToLight = worldToLight * objectToWorld;
        objectToLightScreen[5] = projectionLight * objectToLight;
        worldToLightScreen[5] = projectionLight * worldToLight;

        glProgramUniformMatrix4fv(programShadowCubeMap.id, objectToLightScreen2Location, 6, 0, glm::value_ptr(objectToLightScreen[0]));

        PointLight p =
        {
            pos, 0,
            color,
            1.0,
            {
                worldToLightScreen[0],
                // worldToLightScreen[1],
                // worldToLightScreen[2],
                // worldToLightScreen[3],
                // worldToLightScreen[4],
                // worldToLightScreen[5]
            }
        };
        ((PointLight*) ((int*) lightBuffer + 4))[i] = p;

        // Attach the shadow texture to the depth attachment
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, shadowCubeMapTextures[i], 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubeMapTextures[0], 0);
        // Clear only the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);
        // Render Actor
        planet.render();
        satellite.render();
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    

    glUseProgram(programShadowMap.id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, directionalLightBufferSize, 0, GL_DYNAMIC_COPY);
    lightBuffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    ((int*) lightBuffer)[0] = nbSun;
    for (int i = 0; i < nbSun; ++i)
    {
        glm::vec3 pos(cos(deltaTime * 0.0001 + 6), 0.f, sin(deltaTime * 0.0001 + 6));
        // Light space matrices
        // From light space to shadow map screen space
        // glm::mat4 projectionLight = glm::perspective(glm::radians(45*2.f), 1.f, 1.f, 100.f);
        glm::mat4 projectionLight = glm::ortho(static_cast<float>(-shadowMapSize), static_cast<float>(shadowMapSize), static_cast<float>(shadowMapSize), static_cast<float>(-shadowMapSize), 1.f, 100.f);
        // glm::mat4 projectionLight = glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.f, 100.f);
        // From world to light
        glm::mat4 worldToLight = glm::lookAt(pos, glm::vec3(0), glm::vec3(0.f, 0.f, -1.f));
        // From object to light
        glm::mat4 objectToLight = worldToLight * objectToWorld;
        // From object to shadow map screen space
        glm::mat4 objectToLightScreen = projectionLight * objectToLight;
        // From world to shadow map screen space
        glm::mat4 worldToLightScreen = projectionLight * worldToLight;

        glProgramUniformMatrix4fv(programShadowMap.id, objectToLightScreenLocation, 1, 0, glm::value_ptr(objectToLightScreen));

        DirectionalLight p =
        {
            pos, 0,
            glm::vec3(0.92f, 0.82f, 0.77f),
            .5,
            worldToLightScreen
        };
        ((DirectionalLight*) ((int*) lightBuffer + 4))[i] = p;

        // Attach the shadow texture to the depth attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTextures[i], 0);
        // Clear only the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);
        // Render vaos
        planet.render();
        satellite.render();
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, spotLightBufferSize, 0, GL_DYNAMIC_COPY);
    lightBuffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    ((int*) lightBuffer)[0] = nbRobots;
    for (int i = 0; i < nbRobots; ++i)
    {
        glm::vec3 pos(cos(deltaTime * 0.0002 + .5f * i), .5f, sin(deltaTime * 0.0002 + .5f * i));
        // glm::vec3 pos(0.f, 0.5f, 0.5f);
        glm::vec3 color(fabsf(cos(i*2.f)), 1.-fabsf(sinf(i)) , 0.5f + 0.5f-fabsf(cosf(i)));
        // direction Light to Object
        glm::vec3 dir(glm::normalize(glm::vec3(0, 0, 0) - pos));
        // Light space matrices
        // From light space to shadow map screen space
        glm::mat4 projectionLight = glm::perspective(glm::radians(45*2.f), 1.f, 1.f, 100.f);
        // From world to light
        glm::mat4 worldToLight = glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0.f, 0.f, -1.f));
        // From object to light
        glm::mat4 objectToLight = worldToLight * objectToWorld;
        // From object to shadow map screen space
        glm::mat4 objectToLightScreen = projectionLight * objectToLight;
        // From world to shadow map screen space
        glm::mat4 worldToLightScreen = projectionLight * worldToLight;

        glProgramUniformMatrix4fv(programShadowMap.id, objectToLightScreenLocation, 1, 0, glm::value_ptr(objectToLightScreen));

        SpotLight p =
        {
        pos,
        45,
        glm::vec3(0, -1, 0),
        .5,
        color,
        1.,
        worldToLightScreen
        };
        ((SpotLight*) ((int*) lightBuffer + 4))[i] = p;

        // Attach the shadow texture to the depth attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTextures[nbSun + i], 0);
        // Clear only the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);
        // Render vaos
        planet.render();
        satellite.render();
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ssbo[0], 0, pointLightBufferSize);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, ssbo[1], 0, directionalLightBufferSize);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, ssbo[2], 0, spotLightBufferSize);
    // Fallback to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Revert to window size viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    /////////////////////////////////////
    // Render Object - AOGL in gBuffer //
    /////////////////////////////////////
    // Actors
    // Bind gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);

    glEnable(GL_DEPTH_TEST);
    // Clear the front buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programPlanet.id);
        glProgramUniform2i(programPlanet.id, discretizationLocation, planet.width, planet.height);
        glProgramUniform3f(programPlanet.id, positionLocation, planet.position.x, planet.position.y, planet.position.z);
        glProgramUniform1f(programPlanet.id, radiusLocation, planet.radius);
        planet.render();
        glProgramUniform2i(programPlanet.id, discretizationLocation, satellite.width, satellite.height);
        glProgramUniform3f(programPlanet.id, positionLocation, satellite.position.x, satellite.position.y, satellite.position.z);
        glProgramUniform1f(programPlanet.id, radiusLocation, satellite.radius);
        satellite.render();

    /////////////////////////////////////////////////
    /// Need the GL_DEPTH_TEST to draw the skybox ///
    /// Maybe it's too long to make another pass  ///
    /// And it will be better to use stencil or   ///
    /// another technics that we don't know !     ///
    /////////////////////////////////////////////////
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    // Clear the front buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    // don't use color buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glUseProgram(programPlanet.id);
        glProgramUniform2i(programPlanet.id, discretizationLocation, planet.width, planet.height);
        planet.render();
        glProgramUniform2i(programPlanet.id, discretizationLocation, satellite.width, satellite.height);
        satellite.render();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    //////////////////////////
    // Render FBO -> screen //
    //////////////////////////
    // default frame buffer (screen)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_NEVER);

    glEnable(GL_BLEND);
    // Setup additive blending
    glBlendFunc(GL_ONE, GL_ONE);

    // Textures
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    for(int i = 0; i < shadowCubeMapCount; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + 3 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMapTextures[i]);
    }

    for(int i = 0; i < shadowMapCount; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + 3 + shadowCubeMapCount + i);
        glBindTexture(GL_TEXTURE_2D, shadowMapTextures[i]);
    }

    // Vao screen
    glBindVertexArray(quadVao);

    // PointLight render
    glUseProgram(programPointLight.id);
    for (int i = 0; i < nbBirds; ++i)
    {
        glProgramUniform1i(programPointLight.id, pl_shadowCubeMapLocation, 3 + i);
        glProgramUniform1i(programPointLight.id, pl_IdLocation, i);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }
    // DirectionalLight render
    glUseProgram(programDirectionalLight.id);
    for (int i = 0; i < nbSun; ++i)
    {
        glProgramUniform1i(programDirectionalLight.id, dl_shadowMapLocation, 3 + static_cast<int>(shadowCubeMapCount) + i);
        glProgramUniform1i(programDirectionalLight.id, dl_IdLocation, i);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }
    // SpotLight render
    glUseProgram(programSpotLight.id);
    for (int i = 0; i < nbRobots; ++i)
    {
        glProgramUniform1i(programSpotLight.id, sl_shadowMapLocation, 3 + static_cast<int>(shadowCubeMapCount + nbSun) + i);
        glProgramUniform1i(programSpotLight.id, sl_IdLocation, i);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }

    glDisable(GL_BLEND);

    ///////////////////////////
    // Render Skybox Forward //
    ///////////////////////////
    glEnable(GL_DEPTH_TEST);

    glUseProgram(programSkybox.id);
        skybox.render();

    window.display();
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
