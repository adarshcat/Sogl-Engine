#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace sogl
{
    struct CameraData{
        glm::mat4 viewMatrix;
        glm::mat4 invViewMatrix;
        glm::mat4 viewProjectionMatrix;
        glm::vec3 camPos;
    };
    
    class SoglCamera{
        public:
        SoglCamera(const int width, const int height);

        glm::mat4 getViewProjectionMatrix();
        glm::mat4 getViewMatrix();
        glm::mat4 getInvViewMatrix();

        void setViewMatrix(glm::mat4 viewMat);

        void orbitCamera();

        private:
        const int WIDTH;
        const int HEIGHT;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        float rotDegree = 0.0f;
    };
} // namespace sogl
