#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "engine/sogl_game_object.hpp"

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
        static std::string defaultShader;
        
        static std::vector<std::unique_ptr<SoglGameObject>> loadModel(std::string relativePath);
        static void processNode(aiNode *node, const aiScene *scene, std::vector<std::unique_ptr<SoglGameObject>> &loadedGameObjects);
        static std::unique_ptr<SoglGameObject> processMesh(aiMesh *mesh, const aiScene *scene);
    };
} // namespace sogl
