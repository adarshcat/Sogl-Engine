#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace sogl
{
    struct DirectionalLight{
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        glm::vec3 color;
        glm::vec3 direction;
        float strength;
    };
} // namespace sogl
