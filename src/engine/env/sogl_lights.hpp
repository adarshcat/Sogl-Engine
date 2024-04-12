#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace sogl
{
    struct DirectionalLight{
        glm::mat4 viewProjectionMatrix;

        glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
        glm::vec3 direction = glm::normalize(glm::vec3(2.0f, 1.0f, -1.0f));
        float strength = 3.2f; //1.2f
    };

    namespace LightOperations
    {
        static glm::mat4 adjustShadowMap(DirectionalLight &dirLight, std::vector<glm::vec4> &frustumCorners){
            glm::vec3 center = glm::vec3(0, 0, 0);
            for (const glm::vec4 &corner : frustumCorners){
                center += glm::vec3(corner);
            }
            center /= frustumCorners.size();

            const glm::mat4 lightView = glm::lookAt(
                center + dirLight.direction,
                center,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::lowest();
            for (const glm::vec4 &c : frustumCorners){
                const glm::vec4 trf = lightView * c;
                minX = std::min(minX, trf.x);
                maxX = std::max(maxX, trf.x);
                minY = std::min(minY, trf.y);
                maxY = std::max(maxY, trf.y);
                minZ = std::min(minZ, trf.z);
                maxZ = std::max(maxZ, trf.z);
            }

            constexpr float zMult = 10.0f;
            if (minZ < 0){
                minZ *= zMult;
            }
            else{
                minZ /= zMult;
            }
            if (maxZ < 0){
                maxZ /= zMult;
            }
            else{
                maxZ *= zMult;
            }
            
            const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

            return lightProjection * lightView;
        }
    } // namespace LightOperations
    
} // namespace sogl
