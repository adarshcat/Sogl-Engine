
#include "engine/sogl_engine.hpp"
#include "util/sogl_model_loader.hpp"
#include "engine/sogl_game_object.hpp"

namespace sogl
{
    
    SoglEngine::SoglEngine(): 
        soglWindow{RENDER_WIDTH, RENDER_HEIGHT, "Sogl Window"} ,
        soglCamera(RENDER_WIDTH, RENDER_HEIGHT),
        soglRenderer(soglWindow, RENDER_WIDTH, RENDER_HEIGHT),
        cameraController{SoglCameraController(&soglCamera)}
    {
        soglRenderer.initialiseRenderer();
        
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

    void SoglEngine::addGameObject(SoglGameObject &_gameObj){
        gameObjects.push_back(_gameObj);
    }

    void SoglEngine::addGameObjects(std::vector<SoglGameObject> &_gameObjs){
        gameObjects.insert(gameObjects.end(), _gameObjs.begin(), _gameObjs.end());
    }
} // namespace sogl
