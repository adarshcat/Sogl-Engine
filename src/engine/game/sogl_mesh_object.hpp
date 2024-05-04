#pragma once

#include "engine/game/sogl_game_object.hpp"

namespace sogl
{
    struct Material{
        glm::vec3 albedo = glm::vec3(1.0);
        float roughness = 0.5;
        float metallic = 0.0;
        float alpha = 1.0;
        bool transparent = false;
    };

    struct Vertex{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class SoglMeshObject : public SoglGameObject{
        public:
        SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices, std::string &_shader, std::string &_shaderTransparent, Material &_mat);
        SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices);
        SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices, Material &_mat);
        ~SoglMeshObject();

        virtual void draw(CameraData &camData);
        virtual void drawTransparent(CameraData &camData);
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
        std::string shaderTransparent;
    };
} // namespace sogl
