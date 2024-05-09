
#include "engine/sogl_engine.hpp"
#include "util/sogl_model_loader.hpp"
#include "engine/game/sogl_game_object.hpp"
#include "engine/game/sogl_mesh_object.hpp"

#define DEBUG

namespace sogl
{
    
    SoglEngine::SoglEngine(): 
        soglWindow{RENDER_WIDTH, RENDER_HEIGHT, "Sogl Window"} ,
        soglRenderer(soglWindow, RENDER_WIDTH, RENDER_HEIGHT),
        soglCamera(RENDER_WIDTH, RENDER_HEIGHT),
        cameraController{SoglCameraController(&soglCamera)}
    {
        soglRenderer.initialiseRenderer();

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
                //std::cout << std::to_string(int(1.0f/deltaTime)).c_str() << std::endl;
                for (std::unique_ptr<SoglGameObject> &gameObj : gameObjects){
                    gameObj->process(deltaTime);
                    gameObj->fixedProcess(deltaTime);
                }
                
                lastFixedTick = currentTime;
            }
            else{
                for (std::unique_ptr<SoglGameObject> &gameObj : gameObjects){
                    gameObj->process(deltaTime);
                }
            }

            // construct camera data for renderer
            CameraData camData;
            packCameraData(camData);
            
            // update the camera controller
            cameraController.processInput(soglWindow, deltaTime);
            cameraController.process(deltaTime);
            
            // render to screen
            soglRenderer.draw(gameObjects, camData);


#ifdef DEBUG
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Debug");
            ImGui::LabelText(std::to_string(int(1.0f/deltaTime)).c_str(), "FPS: ");

            ImGui::Checkbox("Rotating", &model0rot);
            if (model0rot)
                gameObjects[0]->rotate(glm::vec3(0, 1, 0), 0.008f);
            
            ImGui::ColorEdit3("color", monkeyCol);
            static_cast<SoglMeshObject*>(gameObjects[0].get())->material.albedo = glm::vec3(monkeyCol[0], monkeyCol[1], monkeyCol[2]);

            ImGui::SliderFloat("roughness", &monkeyRough, 0.0, 1.0);
            static_cast<SoglMeshObject*>(gameObjects[0].get())->material.roughness = monkeyRough;

            ImGui::SliderFloat("metallic", &monkeyMetallic, 0.0, 1.0);
            static_cast<SoglMeshObject*>(gameObjects[0].get())->material.metallic = monkeyMetallic;

            ImGui::Checkbox("Shadows", &shadows);
            soglRenderer.toggleSetting(SoglRenderer::SHADOWS, shadows);

            ImGui::Checkbox("SSAO", &ssao);
            soglRenderer.toggleSetting(SoglRenderer::SSAO, ssao);

            ImGui::Checkbox("SSAO Blur", &ssaoBlur);
            soglRenderer.toggleSetting(SoglRenderer::SSAO_BLUR, ssaoBlur);

            ImGui::SliderFloat("Sun Dir.", &time, 0.0, 3.14*2, "%.4f");
            ImGui::SliderFloat("Sun Strength", &sunStrength, 0.0, 40.0);

            ImGui::Checkbox("ibl", &ibl);
            soglRenderer.toggleSetting(SoglRenderer::IRRADIANCE, ibl);
            
            ImGui::Checkbox("Bloom", &bloomEnabled);

            ImGui::SliderFloat("Bloom Strength", &bloomStrength, 0.0, 0.3, "%.4f");

            ImGui::Checkbox("FXAA", &fxaaEnabled);

            ImGui::ColorEdit3("Color balance", hsvOffset);

            SoglProgramManager::useProgram("postprocessing/pp");
            SoglProgramManager::setFloat("bloomStrength", bloomStrength * bloomEnabled);
            SoglProgramManager::setFloat("fxaaEnabled", fxaaEnabled);
            SoglProgramManager::setVec3("hsvOffset", glm::vec3(hsvOffset[0], hsvOffset[1], hsvOffset[2]));

            DirectionalLight sun;
            sun.direction = glm::normalize(glm::vec3(sin(time)*2.0f, 1.0f, cos(time)*-1.0f));
            sun.strength = sunStrength;
            soglRenderer.updateDirectionalLight(sun);
            
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        } // end engine's draw loop
        while(soglWindow.updateAndPollWindow());

#ifdef DEBUG
        // Free imgui stuff if debug enabled
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
#endif
    } // end run function

    void SoglEngine::packCameraData(CameraData &camData){
        camData.viewProjectionMatrix = soglCamera.getViewProjectionMatrix();
        camData.viewMatrix = soglCamera.getViewMatrix();
        camData.invViewMatrix = soglCamera.getInvViewMatrix();
        camData.projectionMatrix = soglCamera.getProjectionMatrix();
        camData.invProjectionMatrix = soglCamera.getInvProjectionMatrix();
        camData.camPos = cameraController.cameraPos;
        camData.frustumSlice1 = soglCamera.getViewFrustumSlice(3, 0);
        camData.near = soglCamera.NEAR_PLANE;
        camData.far = soglCamera.FAR_PLANE;
    }

#pragma region addObjects
    void SoglEngine::addGameObject(std::unique_ptr<SoglGameObject> &_gameObj){
        gameObjects.push_back(std::move(_gameObj));
    }

    void SoglEngine::addGameObjects(std::vector<std::unique_ptr<SoglGameObject>> _gameObjs){
        for (std::unique_ptr<SoglGameObject> &gameObj : _gameObjs){
            gameObjects.push_back(std::move(gameObj));
        }
    }
#pragma endregion addObjects

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
