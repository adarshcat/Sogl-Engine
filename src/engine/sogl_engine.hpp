#pragma once

#include "sogl_renderer.hpp"
#include "sogl_program_manager.hpp"
#include "sogl_camera_controller.hpp"

// std
#include <vector>

namespace sogl
{
    
    class SoglEngine{
        public:
        SoglEngine();

        void run();

        void addGameObject(SoglGameObject &gameObj);
        
        private:
        const int RENDER_WIDTH = 1024;
        const int RENDER_HEIGHT = 768;

        SoglWindow soglWindow;
        SoglRenderer soglRenderer;
        SoglCamera soglCamera;
        SoglCameraController cameraController;

        std::vector<SoglGameObject> gameObjects;
    };
} // namespace sogl
