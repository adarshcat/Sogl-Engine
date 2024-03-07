#pragma once 

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "sogl_shader.hpp"

namespace sogl
{
    struct Material{
        glm::vec3 albedo;
    };
    
    class SoglGameObject{
        public:
        SoglGameObject(const GLfloat *inData, const int inDataSize, SoglShader *shaderPr, Material mat);

        void draw(glm::mat4 viewProjectionMatrix, glm::vec3 camPos);
        void translate(glm::vec3 amnt);
        void rotate(glm::vec3 axis, float angle);
        void scale(glm::vec3 scaleVec);

        protected:
        void applyMaterial();

        private:
        void initialiseStorageBuffer();

        // All the geometry stuff
        const GLfloat *geometryData;
        const int geometryDataSize;

        GLuint vertexArrayObject;
        SoglShader *shaderProgram;
        Material material;

        // transforms
        glm::mat4 modelMatrix{1.0};
    };
} // namespace sogl
