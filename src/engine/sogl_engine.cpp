
#include "sogl_engine.hpp"
#include "sogl_model_loader.hpp"
#include "sogl_game_object.hpp"

namespace sogl
{
    
    SoglEngine::SoglEngine(): 
        soglWindow{RENDER_WIDTH, RENDER_HEIGHT, "Sogl Window"} ,soglRenderer(soglWindow, RENDER_WIDTH, RENDER_HEIGHT),
        soglCamera(RENDER_WIDTH, RENDER_HEIGHT), shaderManager{SoglShaderManager()}, programManager{SoglProgramManager()}, 
        cameraController{SoglCameraController(&soglCamera)}
    {

        shaderManager.addShader(SoglShader(&programManager));
        SoglModelLoader::defaultShader = &shaderManager.getShader(0);

        // Directly using the engine, not how its supposed to be used. Remove later

        std::vector<SoglGameObject> monkeyModel = SoglModelLoader::loadModel("monkey.obj");
        gameObjects.insert(gameObjects.end(), monkeyModel.begin(), monkeyModel.end());

        std::vector<SoglGameObject> planeModel = SoglModelLoader::loadModel("plane.obj");
        planeModel[0].translate(glm::vec3(0, -2.2, 0));
        gameObjects.insert(gameObjects.end(), planeModel.begin(), planeModel.end());
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
            //gameObjects[0].rotate(glm::vec3(0, 1, 0), 0.001f);

            windowShouldClose = soglRenderer.draw(gameObjects, soglCamera.getViewProjectionMatrix(), cameraController.cameraPos);
        }
        while(!windowShouldClose);
    }

    void SoglEngine::addGameObject(SoglGameObject &gameObj){
        gameObjects.push_back(gameObj);
    }
} // namespace sogl
