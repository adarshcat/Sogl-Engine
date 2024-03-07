#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace sogl
{
    
    class SoglCamera{
        public:
        SoglCamera(const int width, const int height);

        void setViewMatrix(glm::mat4 viewMat);
        glm::mat4 getViewProjectionMatrix();

        void orbitCamera();

        private:
        const int WIDTH;
        const int HEIGHT;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        float rotDegree = 0.0f;
    };
} // namespace sogl
