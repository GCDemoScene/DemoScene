#pragma once

#include <SFML/Graphics.hpp>
#include <GL/glew.h>

#include "Program.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"
#include "Planet.hpp"

#include "lights/PointLight.hpp"
#include "lights/DirectionalLight.hpp"
#include "lights/SpotLight.hpp"

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
    void render(float deltaTime);

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
    Camera camera;
    Skybox skybox;
    Planet planet;
    static const int nbSun = 0;
    DirectionalLight sun[nbSun];
    static const int nbBirds = 1;
    PointLight birds[nbBirds];
    static const int nbRobots = 0;
    SpotLight robots[nbRobots];

    //////////////////
    // Framebuffers //
    //////////////////
    // ---------------
    // Deferred
    // ---------------
    // Quad / Screen 
    // ---------------
    GLuint quadVao;
    GLuint quadVbo[2];
    int quad_triangleCount = 2;
    int quad_triangleList[6] {0, 1, 2, 2, 1, 3};
    float quad_vertices[8] {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};
    // ---------------
    // buffers
    // ---------------
    GLuint gbufferFbo;
    GLuint gbufferDrawBuffers[2];

    GLuint colorTexture;
    GLuint normalTexture;
    GLuint depthTexture;

    GLuint shadowFbo;
    GLuint shadowRenderBuffer;

    // ----------------
    // SSBO - Lights
    // ----------------
    GLuint ssbo[3];

    // --------------
    // ShadowMaps
    // --------------
    int shadowMapSize = 128;
    static const int shadowMapCount = 1 + nbRobots;
    GLuint shadowMapTextures[shadowMapCount];

    static const int shadowCubeMapCount = nbBirds;
    GLuint shadowCubeMapTextures[shadowCubeMapCount];

    // -----------------
    // Programs shaders
    // -----------------
    Program programActor;
    Program programPointLight;
    Program programDirectionalLight;
    Program programSpotLight;
    Program programShadowMap;
    Program programShadowCubeMap;
    Program programSkybox;

    // ---------------
    // Uniforms
    // ---------------
    GLuint timeLocation;
    GLuint mvpLocation;
    GLuint diffuseLocation;
    GLuint specularLocation;
    GLuint specularPowerLocation;
    GLuint discretizationLocation;

    GLuint pl_colorBufferLocation;
    GLuint pl_normalBufferLocation;
    GLuint pl_depthBufferLocation;
    GLuint pl_shadowCubeMapLocation;
    GLuint pl_IdLocation;
    GLuint pl_cameraPositionLocation;
    GLuint pl_inverseProjLocation;
    GLuint pl_mvpLocation;

    GLuint dl_colorBufferLocation;
    GLuint dl_normalBufferLocation;
    GLuint dl_depthBufferLocation;
    GLuint dl_shadowMapLocation;
    GLuint dl_IdLocation;
    GLuint dl_cameraPositionLocation;
    GLuint dl_inverseProjLocation;

    GLuint sl_colorBufferLocation;
    GLuint sl_normalBufferLocation;
    GLuint sl_depthBufferLocation;
    GLuint sl_shadowMapLocation;
    GLuint sl_IdLocation;
    GLuint sl_cameraPositionLocation;
    GLuint sl_inverseProjLocation;

    GLuint objectToLightScreenLocation;
    GLuint objectToLightScreen2Location;

    GLuint skyboxCubeMapLocation;
    GLuint skyboxMVPLocation;
    
    // ---------------
    // Other
    // ---------------
    bool checkError(const char* title);
};
