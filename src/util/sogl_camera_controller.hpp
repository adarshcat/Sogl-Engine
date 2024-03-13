#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "engine/sogl_camera.hpp"
#include "renderer/sogl_window.hpp"

namespace sogl
{
    
    class SoglCameraController{
        public:
        SoglCameraController(SoglCamera* cam);

        void processInput(SoglWindow& window, const float deltaTime);
        glm::vec3 cameraPos = glm::vec3(0, 1, 3);

        private:
        SoglCamera* camera;
        const float cameraSpeed = 10.0f;
        const float mouseSpeed = 5.0f;

        glm::vec3 cameraFront = glm::vec3(0, 0, -1);
        glm::vec3 cameraUp = glm::vec3(0, 1, 0);
        float horizontalAngle = 3.14;
        float verticalAngle = 0;

    };
} // namespace sogl
