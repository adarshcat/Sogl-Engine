//#include "sogl_shader_loader.hpp"
#include "sogl_game_object.hpp"

#include <iostream>

namespace sogl
{
    
    SoglGameObject::SoglGameObject(const GLfloat *inData, const int inVertexDataSize, SoglShader *shaderPr, Material mat): 
        geometryData{inData}, geometryDataSize{inVertexDataSize}, shaderProgram{shaderPr}, material{mat}
    {
        initialiseStorageBuffer();
    }

    void SoglGameObject::initialiseStorageBuffer(){
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);
        // Attach the vertex buffer
        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        // vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(geometryData[0])*geometryDataSize, geometryData, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // normal data
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void SoglGameObject::draw(glm::mat4 viewProjectionMatrix, glm::vec3 camPos){
        applyMaterial();
        shaderProgram->setVec3("cameraPos", camPos);
        shaderProgram->setMat4("MVP", viewProjectionMatrix * modelMatrix);
        shaderProgram->setMat4("M", modelMatrix);
        shaderProgram->applyShader();

        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, geometryDataSize/3);
    }

    void SoglGameObject::applyMaterial(){
        shaderProgram->setVec3("albedo", material.albedo);
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

