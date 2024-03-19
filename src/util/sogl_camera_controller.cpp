
#include "util/sogl_camera_controller.hpp"
#include <cmath>

namespace sogl
{
    SoglCameraController::SoglCameraController(SoglCamera* cam){
        camera = cam;
    }

    void SoglCameraController::process(const float deltaTime){
        horizontalAngle = lerp(horizontalAngle, targetHorizontalAngle, 1.0f - std::pow(cameraRotSmooth, deltaTime));
        verticalAngle = lerp(verticalAngle, targetVerticalAngle, 1.0f - std::pow(cameraRotSmooth, deltaTime));
        
        glm::vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );

        glm::vec3 right = glm::vec3(
            sin(horizontalAngle - 3.14f/2.0f),
            0,
            cos(horizontalAngle - 3.14f/2.0f)
        );

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + direction, cameraUp);
        camera->setViewMatrix(view);
    }

    void SoglCameraController::processInput(SoglWindow &window, const float deltaTime){
        // Enable/Disable camera controller if 'M' key is pressed
        if (glfwGetKey(window.window, GLFW_KEY_M) == GLFW_PRESS && (glfwGetTime() - lastControllerToggle) > 0.1f){
            inputEnabled = !inputEnabled;
            if (inputEnabled)
                glfwSetCursorPos(window.window, window.WIDTH/2, window.HEIGHT/2);
            lastControllerToggle = glfwGetTime();
        }

        if (!inputEnabled) return;

        double xpos, ypos;
        glfwGetCursorPos(window.window, &xpos, &ypos);
        glfwSetCursorPos(window.window, window.WIDTH/2, window.HEIGHT/2);

        targetHorizontalAngle += mouseSpeed * deltaTime * float(window.WIDTH/2 - xpos);
        targetVerticalAngle += mouseSpeed * deltaTime * float(window.HEIGHT/2 - ypos);
        targetVerticalAngle = std::max(double(targetVerticalAngle), -3.14/2);
        targetVerticalAngle = std::min(double(targetVerticalAngle), 3.14/2);

        glm::vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );

        glm::vec3 right = glm::vec3(
            sin(horizontalAngle - 3.14f/2.0f),
            0,
            cos(horizontalAngle - 3.14f/2.0f)
        );

        float apparentCameraSp = cameraSpeed;

        if (glfwGetKey(window.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            apparentCameraSp *= 2.5;
        if (glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += apparentCameraSp * direction * deltaTime;
        if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= apparentCameraSp * direction * deltaTime;
        if (glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= right * apparentCameraSp * deltaTime;
        if (glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += right * apparentCameraSp * deltaTime;
        if (glfwGetKey(window.window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cameraPos += cameraUp * apparentCameraSp * deltaTime;
        if (glfwGetKey(window.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cameraPos -= cameraUp * apparentCameraSp * deltaTime;
    }

    float SoglCameraController::lerp(const float a, const float b, const float c){
        return a + (b - a)*c;
    }
} // namespace sogl
