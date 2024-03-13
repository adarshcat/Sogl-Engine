#pragma once

#include "renderer/sogl_renderer.hpp"
#include "util/shaders/sogl_program_manager.hpp"
#include "util/sogl_camera_controller.hpp"

// std
#include <vector>

namespace sogl
{
    
    class SoglEngine{
        public:
        SoglEngine();

        void run();

        void addGameObject(SoglGameObject &_gameObj);
        void addGameObjects(std::vector<SoglGameObject> &_gameObjs);
        
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
