
#include "engine/sogl_engine.hpp"
#include "util/sogl_model_loader.hpp"
#include "engine/sogl_game_object.hpp"

#define DEBUG

namespace sogl
{
    
    SoglEngine::SoglEngine(): 
        soglWindow{RENDER_WIDTH, RENDER_HEIGHT, "Sogl Window"} ,
        soglCamera(RENDER_WIDTH, RENDER_HEIGHT),
        soglRenderer(soglWindow, RENDER_WIDTH, RENDER_HEIGHT),
        cameraController{SoglCameraController(&soglCamera)}
    {
        soglRenderer.initialiseRenderer();
        soglRenderer.updateDirectionalLight(directionalLight);

        #ifdef DEBUG
        initialiseImguiDebug();
        #endif
    }

    void SoglEngine::run(){
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
            camData.projectionMatrix = soglCamera.getProjectionMatrix();
            camData.camPos = cameraController.cameraPos;
            camData.frustumSlice1 = soglCamera.getViewFrustumSlice(3, 0);
            
            cameraController.processInput(soglWindow, deltaTime);
            cameraController.process(deltaTime);
            
            soglRenderer.draw(gameObjects, camData, directionalLight);


#ifdef DEBUG
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Debug");

            ImGui::Checkbox("Rotating", &model0rot);
            if (model0rot)
                gameObjects[0]->rotate(glm::vec3(0, 1, 0), 0.001f);
            
            ImGui::ColorEdit3("monkeyColor", monkeyCol);
            gameObjects[0]->material.albedo = glm::vec3(monkeyCol[0], monkeyCol[1], monkeyCol[2]);

            ImGui::Checkbox("Shadows", &shadows);
            soglRenderer.toggleShadows(shadows, directionalLight);

            ImGui::Checkbox("SSAO", &ssao);
            soglRenderer.toggleSSAO(ssao);
            
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        }
        while(soglWindow.updateAndPollWindow());

#ifdef DEBUG
        // Free imgui stuff if debug enabled
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
#endif
    }


    void SoglEngine::addGameObject(std::unique_ptr<SoglGameObject> &_gameObj){
        gameObjects.push_back(std::move(_gameObj));
    }

    void SoglEngine::addGameObjects(std::vector<std::unique_ptr<SoglGameObject>> _gameObjs){
        for (std::unique_ptr<SoglGameObject> &gameObj : _gameObjs){
            gameObjects.push_back(std::move(gameObj));
        }
    }


    // DEBUG
    #ifdef DEBUG
    void SoglEngine::initialiseImguiDebug(){
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(soglWindow.window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }
    #endif
} // namespace sogl
