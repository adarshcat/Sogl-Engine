#pragma once 

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/env/sogl_camera.hpp"


//std
#include <iostream>
#include <vector>
#include <string>

namespace sogl
{
    
    class SoglGameObject{
        public:
        SoglGameObject(){}
        SoglGameObject(glm::mat4 _transform);

        virtual void draw(CameraData &camData){}
        virtual void drawShadow(glm::mat4 &lightSpaceMatrix){}

        virtual void process(const float delta){}
        virtual void fixedProcess(const float delta){}

        // basic transformations
        void translate(glm::vec3 _amnt);
        void rotate(glm::vec3 _axis, float _angle);
        void scale(glm::vec3 _scaleVec);

        void setTransform(glm::mat4 _transform);

        glm::mat4 modelMatrix{1.0};
    };
} // namespace sogl
