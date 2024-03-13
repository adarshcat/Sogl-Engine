
#include "util/sogl_camera_controller.hpp"
#include <cmath>

namespace sogl
{
    SoglCameraController::SoglCameraController(SoglCamera* cam){
        camera = cam;
    }

    void SoglCameraController::processInput(SoglWindow &window, const float deltaTime){
        double xpos, ypos;
        glfwGetCursorPos(window.window, &xpos, &ypos);
        glfwSetCursorPos(window.window, window.WIDTH/2, window.HEIGHT/2);

        horizontalAngle += mouseSpeed * deltaTime * float(window.WIDTH/2 - xpos);
        verticalAngle += mouseSpeed * deltaTime * float(window.HEIGHT/2 - ypos);
        verticalAngle = std::max(double(verticalAngle), -3.14/2);
        verticalAngle = std::min(double(verticalAngle), 3.14/2);

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

        glm::vec3 up = glm::cross(right, direction);

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
        

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + direction, cameraUp);
        camera->setViewMatrix(view);
    }
} // namespace sogl
