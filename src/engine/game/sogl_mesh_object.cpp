
#include "engine/game/sogl_mesh_object.hpp"

//std
#include <iostream>

namespace sogl
{
    
    SoglMeshObject::SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices, std::string &_shader, std::string &_shaderTransparent, Material &_mat): 
            vertices{_inVertices}, indices{_inIndices}, shader{_shader}, shaderTransparent{_shaderTransparent}, material{_mat}
    {
        initialiseShader();
        initialiseStorageBuffer();
    }

    SoglMeshObject::SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices): 
            vertices{_inVertices}, indices{_inIndices}, shader{SoglProgramManager::defaultShader}, 
            shaderTransparent{SoglProgramManager::defaultShaderTransparent}, material{Material()}
    {
        initialiseShader();
        initialiseStorageBuffer();
    }

    SoglMeshObject::SoglMeshObject(std::vector<Vertex> &_inVertices, std::vector<unsigned int> &_inIndices, Material &_mat): 
            vertices{_inVertices}, indices{_inIndices}, shader{SoglProgramManager::defaultShader},
            shaderTransparent{SoglProgramManager::defaultShaderTransparent}, material{_mat}
    {
        initialiseShader();
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

    void SoglMeshObject::initialiseShader(){
        // initialise the transparent shader
        SoglProgramManager::addProgram(shaderTransparent);
        SoglProgramManager::useProgram(shaderTransparent);

        SoglProgramManager::bindImage("gDepth", 0);
        SoglProgramManager::bindImage("dirLight.shadowMap", 1);
        SoglProgramManager::bindImage("skyIrradiance", 2);
        SoglProgramManager::bindImage("prefilterMap", 3);
        SoglProgramManager::bindImage("brdfLUT", 4);
        
        // initialise the default shader
        SoglProgramManager::addProgram(shader);

        // initialise corresponding shadow shader
        SoglProgramManager::addProgram(shader+SoglProgramManager::shadowSuffix);
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

    void SoglMeshObject::drawTransparent(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, DirectionalLight &dirLight){
        SoglProgramManager::useProgram(shaderTransparent);
        applyMaterial();

        // attach needed textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lightingData.gDepth);
        if (lightingSettings.shadowEnabled){
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, lightingData.shadowMap);
        }
        if (lightingSettings.irradianceEnabled){
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, lightingData.skyboxDiffuseIrradiance);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_CUBE_MAP, lightingData.skyboxPrefilterMap);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, lightingData.skyboxBrdfLUT);
        }
        // -----------------------

        // update needed params
        SoglProgramManager::setMat4("camera.invView", lightingData.camData.invViewMatrix);
        SoglProgramManager::setMat4("camera.invProjection", lightingData.camData.invProjectionMatrix);
        SoglProgramManager::setVec3("camera.position", lightingData.camData.camPos);
        SoglProgramManager::setFloat("camera.near", lightingData.camData.near);
        SoglProgramManager::setFloat("camera.far", lightingData.camData.far);

        if (lightingSettings.shadowEnabled) SoglProgramManager::setMat4("dirLight.transformMatrix", lightingData.dirLightMatrix);

        SoglProgramManager::setVec3("dirLight.color", dirLight.color);
        SoglProgramManager::setVec3("dirLight.direction", glm::normalize(dirLight.direction));
        SoglProgramManager::setFloat("dirLight.strength", dirLight.strength);

        SoglProgramManager::setMat4("mvpMatrix", lightingData.camData.viewProjectionMatrix * modelMatrix);
        SoglProgramManager::setMat4("mvMatrix", lightingData.camData.viewMatrix * modelMatrix);
        SoglProgramManager::setMat4("modelMatrix", modelMatrix);
        // ---------------------

        // update shader settings
        SoglProgramManager::setInt("shadowEnabled", lightingSettings.shadowEnabled);
        SoglProgramManager::setInt("irradianceEnabled", lightingSettings.irradianceEnabled);
        // ----------------------

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

        // transparency
        if (material.transparent)
            SoglProgramManager::setFloat("alpha", material.alpha);
    }

} // namespace sogl