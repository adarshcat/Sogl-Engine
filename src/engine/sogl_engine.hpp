#pragma once

#include "renderer/sogl_renderer.hpp"
#include "util/shaders/sogl_program_manager.hpp"
#include "util/sogl_camera_controller.hpp"

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
        const int RENDER_WIDTH = 1024;
        const int RENDER_HEIGHT = 768;

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
        float monkeyCol[3]{1.0f, 1.0f, 1.0f};
        bool ssaoBlur = true;

        float radiusSSAO = 0.5;
        float biasSSAO = 0.08;
        float time = 3.45;
#endif
    };
} // namespace sogl
