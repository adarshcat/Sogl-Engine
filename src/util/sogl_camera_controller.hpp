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
        void process(const float deltaTime);

        glm::vec3 cameraPos = glm::vec3(0, 1, 12);
        bool inputEnabled = true;

        private:
        SoglCamera* camera;
        const float cameraSpeed = 14.0f;
        const float mouseSpeed = 5.0f;
        const float cameraRotSmooth = 0.00001f;

        float lerp(const float a, const float b, const float c);

        const glm::vec3 cameraFront = glm::vec3(0, 0, -1);
        const glm::vec3 cameraUp = glm::vec3(0, 1, 0);

        float lastControllerToggle = 0.0f;
        float targetHorizontalAngle = 3.14f;
        float targetVerticalAngle = 0.0f;
        float horizontalAngle = targetHorizontalAngle;
        float verticalAngle = targetVerticalAngle;

    };
} // namespace sogl
