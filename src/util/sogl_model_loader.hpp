#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "engine/sogl_game_object.hpp"
#include "engine/sogl_mesh_object.hpp"
#include "util/shaders/sogl_program_manager.hpp"

//std
#include <vector>
#include <string>
#include <iostream>
#include <memory>

namespace sogl
{
    
    class SoglModelLoader{
        public:
        static std::string modelRoot;
        
        static std::vector<std::unique_ptr<SoglGameObject>> loadModel(std::string relativePath);
        static void processNode(aiNode *node, glm::mat4 transform, const aiScene *scene, std::vector<std::unique_ptr<SoglGameObject>> &loadedGameObjects);
        static std::unique_ptr<SoglGameObject> processMesh(aiMesh *mesh, const aiScene *scene);

        static glm::mat4 aiMatrixToGLM(aiMatrix4x4 from);
    };
} // namespace sogl
