#pragma once 

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/sogl_camera.hpp"


//std
#include <vector>
#include <string>

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
        SoglGameObject(std::vector<Vertex> &inVertices, std::vector<unsigned int> &inIndices, std::string &_shader, Material &mat);

        void draw(CameraData &camData);
        void drawShadow(glm::mat4 &lightSpaceMatrix);

        void translate(glm::vec3 amnt);
        void rotate(glm::vec3 axis, float angle);
        void scale(glm::vec3 scaleVec);

        Material material;

        protected:
        void applyMaterial();

        private:
        void initialiseStorageBuffer();

        // All the geometry stuff
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint vertexArrayObject;

        std::string shader;

        // transforms
        glm::mat4 modelMatrix{1.0};
    };
} // namespace sogl
