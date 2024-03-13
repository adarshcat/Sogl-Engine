
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
        projectionMatrix = glm::perspective(glm::radians(45.0f), (float) WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    }

    void SoglCamera::setViewMatrix(glm::mat4 viewMat){
        viewMatrix = viewMat;
    }

    glm::mat4 SoglCamera::getViewProjectionMatrix(){
        return projectionMatrix * viewMatrix;
    }


    void SoglCamera::orbitCamera(){
        viewMatrix = glm::lookAt(
            glm::vec3(sinf(rotDegree)*4,3,cosf(rotDegree)*4),
            glm::vec3(0,0,0),
            glm::vec3(0,1,0)
        );

        rotDegree += 0.001;
    }
} // namespace sogl
