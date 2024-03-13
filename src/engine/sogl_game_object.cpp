//#include "sogl_shader_loader.hpp"
#include "engine/sogl_game_object.hpp"
#include "util/shaders/sogl_program_manager.hpp"

//std
#include <iostream>

namespace sogl
{
    
    SoglGameObject::SoglGameObject(std::vector<Vertex> inVertices, std::vector<unsigned int> inIndices, std::string _shader, Material mat): 
        vertices{inVertices}, indices{inIndices}, shader{_shader}, material{mat}
    {
        SoglProgramManager::addProgram(shader);
        initialiseStorageBuffer();
    }

    void SoglGameObject::initialiseStorageBuffer(){
        GLuint vertexBuffer;
        GLuint elementBuffer;

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

    void SoglGameObject::draw(glm::mat4 viewProjectionMatrix, glm::vec3 camPos){
        SoglProgramManager::useProgram(shader);
        applyMaterial();
        SoglProgramManager::setVec3("cameraPos", camPos);
        SoglProgramManager::setMat4("MVP", viewProjectionMatrix * modelMatrix);
        SoglProgramManager::setMat4("M", modelMatrix);

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    void SoglGameObject::applyMaterial(){
        SoglProgramManager::setVec3("albedo", material.albedo);
    }

    void SoglGameObject::translate(glm::vec3 amnt){
        modelMatrix = glm::translate(modelMatrix, amnt);
    }

    void SoglGameObject::rotate(glm::vec3 axis, float angle){
        modelMatrix = glm::rotate(modelMatrix, angle, axis);
    }

    void SoglGameObject::scale(glm::vec3 scaleVec){
        modelMatrix = glm::scale(modelMatrix, scaleVec);
    }

} // namespace sogl

