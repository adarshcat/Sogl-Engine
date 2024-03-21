#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace sogl
{
    struct CameraData{
        glm::mat4 viewMatrix;
        glm::mat4 invViewMatrix;
        glm::mat4 viewProjectionMatrix;
        glm::mat4 projectionMatrix;
        std::vector<glm::vec4> frustumSlice1;
        glm::vec3 camPos;
    };
    
    class SoglCamera{
        public:
        SoglCamera(const int width, const int height);

        glm::mat4 getViewProjectionMatrix();
        glm::mat4 &getViewMatrix();
        glm::mat4 &getProjectionMatrix();
        glm::mat4 getInvViewMatrix();

        std::vector<glm::vec4> getViewFrustum(glm::mat4 &viewProjMatrix);
        std::vector<glm::vec4> getViewFrustumSlice(const int div, const int offset);

        void setViewMatrix(glm::mat4 &viewMat);

        private:
        const int WIDTH;
        const int HEIGHT;

        const float NEAR_PLANE = 0.01f;
        const float FAR_PLANE = 100.0f;
        const float FOV = 45.0f; // in degrees

        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        float rotDegree = 0.0f;
    };
} // namespace sogl
