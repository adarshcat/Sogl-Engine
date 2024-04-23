#pragma once

#include "renderer/sogl_renderer.hpp"
#include "util/shaders/sogl_program_manager.hpp"
#include "util/sogl_camera_controller.hpp"
#include "engine/env/sogl_lights.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// std
#include <vector>
#include <memory>

#define DEBUG

namespace sogl
{
    
    class SoglEngine{
        public:
        SoglEngine();

        void run();

        void addGameObject(std::unique_ptr<SoglGameObject> &_gameObj);
        void addGameObjects(std::vector<std::unique_ptr<SoglGameObject>> _gameObjs);
        
        private:
        const int RENDER_WIDTH = 1224; //1024
        const int RENDER_HEIGHT = 768; //768

        void initialiseImguiDebug();
        void packCameraData(CameraData &camData);

        SoglWindow soglWindow;
        SoglRenderer soglRenderer;
        SoglCamera soglCamera;
        SoglCameraController cameraController;

        std::vector<std::unique_ptr<SoglGameObject>> gameObjects;

#ifdef DEBUG
        bool model0rot = false;
        bool shadows = true;
        bool ssao = true;
        bool ssaoBlur = true;
        
        float monkeyCol[3]{1.0f, 1.0f, 1.0f};
        float monkeyRough = 0.5;
        float monkeyMetallic = 1.0;

        float time = 3.45;
        float sunStrength = 8.0;

        bool ibl = true;
#endif
    };
} // namespace sogl
