
#include "engine/sogl_mesh_object.hpp"
#include "util/shaders/sogl_program_manager.hpp"

//std
#include <iostream>

namespace sogl
{
    
    SoglMeshObject::SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices, std::string &_shader, Material &_mat): 
            vertices{_inVertices}, indices{_inIndices}, shader{_shader}, material{_mat}
    {
        SoglProgramManager::addProgram(shader);
        SoglProgramManager::addProgram(shader+SoglProgramManager::shadowSuffix);
        initialiseStorageBuffer();
    }

    SoglMeshObject::SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices): 
            vertices{_inVertices}, indices{_inIndices}, shader{SoglProgramManager::defaultShader}, material{Material()}
    {
        SoglProgramManager::addProgram(shader);
        SoglProgramManager::addProgram(shader+SoglProgramManager::shadowSuffix);
        initialiseStorageBuffer();
    }

    SoglMeshObject::SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices, Material &_mat): 
            vertices{_inVertices}, indices{_inIndices}, shader{SoglProgramManager::defaultShader}, material{_mat}
    {
        SoglProgramManager::addProgram(shader);
        SoglProgramManager::addProgram(shader+SoglProgramManager::shadowSuffix);
        initialiseStorageBuffer();
    }

    SoglMeshObject::~SoglMeshObject(){
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &elementBuffer);
        glDeleteVertexArrays(1, &vertexArrayObject);
    }

    void SoglMeshObject::initialiseStorageBuffer(){
        glGenVertexArrays(1, &vertexArrayObject);
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &elementBuffer);

        glBindVertexArray(vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }

    
#pragma region drawFunctions
    void SoglMeshObject::draw(CameraData &camData){
        SoglProgramManager::useProgram(shader);
        applyMaterial();
        SoglProgramManager::setMat4("mvpMatrix", camData.viewProjectionMatrix * modelMatrix);
        SoglProgramManager::setMat4("mvMatrix", camData.viewMatrix * modelMatrix);
        SoglProgramManager::setMat4("modelMatrix", modelMatrix);
        SoglProgramManager::setFloat("camera.near", camData.near);
        SoglProgramManager::setFloat("camera.far", camData.far);

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    void SoglMeshObject::drawShadow(glm::mat4 &lightSpaceMatrix){
        SoglProgramManager::useProgram(shader+"_shadow");
        applyMaterial();
        SoglProgramManager::setMat4("modelMatrix", modelMatrix);
        SoglProgramManager::setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
#pragma endregion drawFunctions

    void SoglMeshObject::applyMaterial(){
        SoglProgramManager::setVec3("albedo", material.albedo);
        SoglProgramManager::setFloat("roughness", material.roughness);
        SoglProgramManager::setFloat("metallic", material.metallic);
    }

} // namespace sogl