#pragma once

#include "engine/sogl_game_object.hpp"

namespace sogl
{
    struct Material{
        glm::vec3 albedo;
    };

    struct Vertex{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class SoglMeshObject : public SoglGameObject{
        public:
        SoglMeshObject(std::vector<Vertex> &inVertices, std::vector<unsigned int> &inIndices, std::string &_shader, Material &mat);
        ~SoglMeshObject();

        virtual void draw(CameraData &camData);
        virtual void drawShadow(glm::mat4 &lightSpaceMatrix);

        Material material;

        private:
        void initialiseStorageBuffer();

        protected:
        void applyMaterial();

        // All the geometry stuff
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        GLuint vertexArrayObject;
        GLuint vertexBuffer;
        GLuint elementBuffer;

        std::string shader;
    };
} // namespace sogl
