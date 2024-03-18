#pragma once

#include "renderer/sogl_renderer.hpp"
#include "util/shaders/sogl_program_manager.hpp"
#include "util/sogl_camera_controller.hpp"

// std
#include <vector>
#include <memory>

namespace sogl
{
    
    class SoglEngine{
        public:
        SoglEngine();

        void run();

        void addGameObject(std::unique_ptr<SoglGameObject> &_gameObj);
        void addGameObjects(std::vector<std::unique_ptr<SoglGameObject>> _gameObjs);

        DirectionalLight directionalLight;
        
        private:
        const int RENDER_WIDTH = 1024;
        const int RENDER_HEIGHT = 768;

        SoglWindow soglWindow;
        SoglRenderer soglRenderer;
        SoglCamera soglCamera;
        SoglCameraController cameraController;

        std::vector<std::unique_ptr<SoglGameObject>> gameObjects;
    };
} // namespace sogl
