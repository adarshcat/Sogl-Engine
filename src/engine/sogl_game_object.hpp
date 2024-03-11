#pragma once 

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "sogl_shader.hpp"

//std
#include <vector>

namespace sogl
{
    struct Material{
        glm::vec3 albedo;
    };

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
    
    class SoglGameObject{
        public:
        SoglGameObject(std::vector<Vertex> inVertices, std::vector<unsigned int> inIndices, SoglShader *shaderPr, Material mat);

        void draw(glm::mat4 viewProjectionMatrix, glm::vec3 camPos);
        void translate(glm::vec3 amnt);
        void rotate(glm::vec3 axis, float angle);
        void scale(glm::vec3 scaleVec);

        protected:
        void applyMaterial();

        private:
        void initialiseStorageBuffer();

        // All the geometry stuff
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint vertexArrayObject;

        SoglShader *shaderProgram;
        Material material;

        // transforms
        glm::mat4 modelMatrix{1.0};
    };
} // namespace sogl
