#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "engine/sogl_game_object.hpp"

//std
#include <vector>
#include <string>
#include <iostream>

namespace sogl
{
    
    class SoglModelLoader{
        public:
        static std::string modelRoot;
        static std::string defaultShader;
        
        static std::vector<SoglGameObject> loadModel(std::string relativePath);
        static void processNode(aiNode *node, const aiScene *scene, std::vector<SoglGameObject> &loadedGameObjects);
        static SoglGameObject processMesh(aiMesh *mesh, const aiScene *scene);
    };
} // namespace sogl
