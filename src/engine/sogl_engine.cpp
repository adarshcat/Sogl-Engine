
#include "sogl_engine.hpp"
#include "sogl_game_object.hpp"

namespace sogl
{
    
    SoglEngine::SoglEngine(): 
        soglWindow{RENDER_WIDTH, RENDER_HEIGHT, "Sogl Window"} ,soglRenderer(soglWindow, RENDER_WIDTH, RENDER_HEIGHT),
        soglCamera(RENDER_WIDTH, RENDER_HEIGHT), shaderManager{SoglShaderManager()}, programManager{SoglProgramManager()}, 
        cameraController{SoglCameraController(&soglCamera)}
    {

        // Directly using the engine, not how its supposed to be used. Remove later

        const GLfloat cubeData[] = {
            -1.0f,-1.0f,-1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f,-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f,-1.0f,  0.0f, 0.0f, -1.0f,
            -1.0f,-1.0f,-1.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, 1.0f,-1.0f, 0.0f, 0.0f, -1.0f,
            1.0f,-1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
            -1.0f,-1.0f,-1.0f, 0.0f, -1.0f, 0.0f,
            1.0f,-1.0f,-1.0f,  0.0f, -1.0f, 0.0f,
            1.0f, 1.0f,-1.0f,  0.0f, 0.0f, -1.0f,
            1.0f,-1.0f,-1.0f,  0.0f, 0.0f, -1.0f,
            -1.0f,-1.0f,-1.0f, 0.0f, 0.0f, -1.0f,
            -1.0f,-1.0f,-1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f,-1.0f, -1.0f, 0.0f, 0.0f,
            1.0f,-1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
            -1.0f,-1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            -1.0f,-1.0f,-1.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f,-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
            1.0f,-1.0f,-1.0f,  1.0f, 0.0f, 0.0f,
            1.0f, 1.0f,-1.0f,  1.0f, 0.0f, 0.0f,
            1.0f,-1.0f,-1.0f,  1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
            1.0f,-1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
            1.0f, 1.0f,-1.0f,  0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f,-1.0f, 1.0f,   0.0f, 0.0f, 1.0f
        };

        float span = 5.0f;
        float y = -1.75f;
        const GLfloat planeData[] = {
            -span, y, -span, 0.0f, 1.0f, 0.0f,
            span, y, -span,  0.0f, 1.0f, 0.0f,
            -span, y, span,  0.0f, 1.0f, 0.0f,

            span, y, -span,  0.0f, 1.0f, 0.0f,
            span, y, span,   0.0f, 1.0f, 0.0f,
            -span, y, span,  0.0f, 1.0f, 0.0f
        };

        shaderManager.addShader(SoglShader(&programManager));

        Material mat1;
        mat1.albedo = glm::vec3(1.0, 0.0, 0.0);

        Material mat2;
        mat2.albedo = glm::vec3(0.0, 1.0, 0.0);

        gameObjects.push_back(SoglGameObject(cubeData, sizeof(cubeData)/sizeof(GLfloat), &shaderManager.getShader(0), mat1));
        gameObjects.push_back(SoglGameObject(planeData, sizeof(planeData)/sizeof(GLfloat), &shaderManager.getShader(0), mat2));
    }

    void SoglEngine::run(){
        bool windowShouldClose = false;
        float deltaTime = 0.0;
        float lastFrame = 0.0;
        do {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            cameraController.processInput(soglWindow, deltaTime);
            gameObjects[0].rotate(glm::vec3(0, 1, 0), 0.001f);

            windowShouldClose = soglRenderer.draw(gameObjects, soglCamera.getViewProjectionMatrix(), cameraController.cameraPos);
        }
        while(!windowShouldClose);
    }

    void SoglEngine::addGameObject(SoglGameObject &gameObj){
        gameObjects.push_back(gameObj);
    }
} // namespace sogl
