
#include "engine/sogl_camera.hpp"
#include <cmath>

namespace sogl
{
    
    SoglCamera::SoglCamera(const int width, const int height): WIDTH{width}, HEIGHT{height}{
        // Initialise the view matrix at 4,3,3 looking at origin
        viewMatrix = glm::lookAt(
            glm::vec3(4,3,3),
            glm::vec3(0,0,0),
            glm::vec3(0,1,0)
        );

        // Initialise simple projection matrix with fov of 45 degrees, near and far
        projectionMatrix = glm::perspective(glm::radians(FOV), (float) WIDTH / (float)HEIGHT, NEAR_PLANE, FAR_PLANE);
    }

#pragma region getterFunctions
    glm::mat4 SoglCamera::getViewProjectionMatrix(){
        return projectionMatrix * viewMatrix;
    }

    glm::mat4 &SoglCamera::getViewMatrix(){
        return viewMatrix;
    }

    glm::mat4 &SoglCamera::getProjectionMatrix(){
        return projectionMatrix;
    }

    glm::mat4 SoglCamera::getInvViewMatrix(){
        return glm::inverse(viewMatrix);
    }

    std::vector<glm::vec4> SoglCamera::getViewFrustum(glm::mat4 &viewProjMatrix){
        const glm::mat4 invViewProjection = glm::inverse(viewProjMatrix);

        std::vector<glm::vec4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x){
            for (unsigned int y = 0; y < 2; ++y){
                for (unsigned int z = 0; z < 2; ++z){
                    const glm::vec4 pt = 
                        invViewProjection * glm::vec4(
                            2.0f * x - 1.0f,
                            2.0f * y - 1.0f,
                            2.0f * z - 1.0f,
                            1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    std::vector<glm::vec4> SoglCamera::getViewFrustumSlice(const int div, const int offset){
        const float SLICE_DEPTH = (FAR_PLANE - NEAR_PLANE) / div;

        glm::mat4 slicedProjectionMatrix = glm::perspective(glm::radians(FOV), (float) WIDTH / (float)HEIGHT,
            NEAR_PLANE + SLICE_DEPTH*offset, NEAR_PLANE + SLICE_DEPTH*(offset+1));
        
        glm::mat4 slicedViewProjMatrix = slicedProjectionMatrix * viewMatrix;

        return getViewFrustum(slicedViewProjMatrix);
    }
#pragma endregion getterFunctions

    void SoglCamera::setViewMatrix(glm::mat4 &viewMat){
        viewMatrix = viewMat;
    }
} // namespace sogl
