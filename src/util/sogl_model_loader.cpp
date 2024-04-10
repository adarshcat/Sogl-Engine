
#include "util/sogl_model_loader.hpp"

namespace sogl
{
    std::string SoglModelLoader::modelRoot = "assets/models/";
    std::string SoglModelLoader::defaultShader = "default";
    
    std::vector<std::unique_ptr<SoglGameObject>> SoglModelLoader::loadModel(std::string relativePath){
        std::string path = modelRoot + relativePath;
        std::vector<std::unique_ptr<SoglGameObject>> loadedGameObjects;

        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
        
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        }

        processNode(scene->mRootNode, glm::mat4(1.0), scene, loadedGameObjects);

        return loadedGameObjects;
    }

    void SoglModelLoader::processNode(aiNode *node, glm::mat4 transform, const aiScene *scene, std::vector<std::unique_ptr<SoglGameObject>> &loadedGameObjects){
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            std::unique_ptr<SoglGameObject> meshObject = processMesh(mesh, scene);
            meshObject->setTransform(transform);
            loadedGameObjects.push_back(std::move(meshObject));
        }
        
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            glm::mat4 localTransform = aiMatrixToGLM(node->mTransformation);
            processNode(node->mChildren[i], localTransform * transform, scene, loadedGameObjects);
        }
    }

    std::unique_ptr<SoglGameObject> SoglModelLoader::processMesh(aiMesh *mesh, const aiScene *scene) {
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
        
        aiColor3D albedo (1.0, 1.0, 1.0);
        // process material
        if(mesh->mMaterialIndex >= 0) {
            scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, albedo);
        }

        Material mat;
        mat.albedo = glm::vec3(albedo.r, albedo.g, albedo.b);

        return std::unique_ptr<SoglGameObject>(new SoglMeshObject(vertices, indices, defaultShader, mat));
    }

    glm::mat4 SoglModelLoader::aiMatrixToGLM(aiMatrix4x4 from){
        glm::mat4 to;

        to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
        to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
        to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
        to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

        return to;
    }
} // namespace sogl
