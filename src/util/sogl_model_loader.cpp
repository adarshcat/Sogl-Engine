
#include "sogl_model_loader.hpp"

namespace sogl
{
    std::string SoglModelLoader::modelRoot = "assets/models/";
    SoglShader *SoglModelLoader::defaultShader = nullptr;
    
    std::vector<SoglGameObject> SoglModelLoader::loadModel(std::string relativePath){
        std::string path = modelRoot + relativePath;
        std::vector<SoglGameObject> loadedGameObjects;

        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
        
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        }
        //directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene, loadedGameObjects);

        return loadedGameObjects;
    }

    void SoglModelLoader::processNode(aiNode *node, const aiScene *scene, std::vector<SoglGameObject> &loadedGameObjects){
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            loadedGameObjects.push_back(processMesh(mesh, scene));
        }
        
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, loadedGameObjects);
        }
    }

    SoglGameObject SoglModelLoader::processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        //std::vector<Texture> textures;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            // process vertex positions, normals and texture coordinates
            glm::vec3 vector; 
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z; 
            vertex.Position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            if(mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);  

            vertices.push_back(vertex);
        }
        // process indices

        for(unsigned int i = 0; i < mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
                
        // process material
        /*if(mesh->mMaterialIndex >= 0) {
            [...]
        }*/

        Material mat;
        mat.albedo = glm::vec3(1.0);

        return SoglGameObject(vertices, indices, defaultShader, mat);
    }  
} // namespace sogl
