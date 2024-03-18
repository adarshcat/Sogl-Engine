
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
        soglRenderer.initialiseLighting(directionalLight);
    }

    void SoglEngine::run(){
        bool windowShouldClose = false;
        float deltaTime = 0.0;
        float lastTick = 0.0;
        float lastFixedTick = 0.0;
        float fixedLoopInterval = 1.0/60;

        do {
            // delta time calculation
            float currentTime = glfwGetTime();
            deltaTime = currentTime - lastTick;
            lastTick = currentTime;

            // Fixed loop, runs a fixed number of times a second
            float lastFixedLoopTime = currentTime - lastFixedTick;
            if (lastFixedLoopTime > fixedLoopInterval){
                // fixed loop stuff goes here
                std::cout << "FPS: " << int(1.0f/deltaTime) << std::endl;
                lastFixedTick = currentTime;
            }

            // construct camera data for renderer
            CameraData camData;
            camData.viewProjectionMatrix = soglCamera.getViewProjectionMatrix();
            camData.viewMatrix = soglCamera.getViewMatrix();
            camData.invViewMatrix = soglCamera.getInvViewMatrix();
            camData.camPos = cameraController.cameraPos;
            camData.frustumSlice1 = soglCamera.getViewFrustumSlice(3, 0);
            
            cameraController.processInput(soglWindow, deltaTime);

            gameObjects[0]->rotate(glm::vec3(0, 1, 0), 0.001f);

            windowShouldClose = soglRenderer.draw(gameObjects, camData, directionalLight);
        }
        while(!windowShouldClose);
    }


    void SoglEngine::addGameObject(std::unique_ptr<SoglGameObject> &_gameObj){
        gameObjects.push_back(std::move(_gameObj));
    }

    void SoglEngine::addGameObjects(std::vector<std::unique_ptr<SoglGameObject>> _gameObjs){
        for (std::unique_ptr<SoglGameObject> &gameObj : _gameObjs){
            gameObjects.push_back(std::move(gameObj));
        }
        //gameObjects.insert(gameObjects.end(), _gameObjs.begin(), _gameObjs.end());
    }
} // namespace sogl
