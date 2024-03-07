#pragma once

#include "sogl_renderer.hpp"
#include "sogl_shader_manager.hpp"
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
        void createMaterial(); // TODO: creates a default material and adds it to the material manager (probably also uses hashmap to assign it a name for future uses) 

        private:
        const int RENDER_WIDTH = 1024;
        const int RENDER_HEIGHT = 768;

        SoglWindow soglWindow;
        SoglRenderer soglRenderer;
        SoglCamera soglCamera;
        SoglCameraController cameraController;
        SoglShaderManager shaderManager;
        SoglProgramManager programManager;

        std::vector<SoglGameObject> gameObjects;
    };
} // namespace sogl
