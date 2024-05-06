
#include "renderer/modules/sogl_lighting_module.hpp"

namespace sogl
{
    
    SoglLightingModule::SoglLightingModule(const int _width, const int _height): WIDTH{_width}, HEIGHT{_height}{}
    SoglLightingModule::~SoglLightingModule(){

    }

    void SoglLightingModule::initialiseLighting(GLuint renderFBO){
        // bind render frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

        // generate the output texture
        glGenTextures(1, &outputTex);
        glBindTexture(GL_TEXTURE_2D, outputTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLuint rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        SoglLightingSettings ls;
        updateLighting(ls);
    }


    void SoglLightingModule::lightingPass(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, GLuint renderFBO, GLuint quadVAO){
        glViewport(0, 0, WIDTH, HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

        glEnable(GL_DEPTH_TEST);
        defferedShading(lightingData, lightingSettings, quadVAO);
        forwardShading(lightingData, lightingSettings, gameObjects, renderFBO);
        glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SoglLightingModule::defferedShading(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, GLuint quadVAO){
        SoglProgramManager::useProgram(lightingShader);

        // bind the g-buffer to lighting shader program
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lightingData.gDepth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lightingData.gNormalMet);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, lightingData.gAlbedoSpec);

        if (lightingSettings.shadowEnabled){
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, lightingData.shadowMap);
        }
        
        if (lightingSettings.ssaoEnabled){
            glActiveTexture(GL_TEXTURE4);
            if (lightingSettings.ssaoBlurEnabled)
                glBindTexture(GL_TEXTURE_2D, lightingData.ssaoBlurOutput);
            else
                glBindTexture(GL_TEXTURE_2D, lightingData.ssaoOutput);
        }

        if (lightingSettings.irradianceEnabled){
            //attach skybox diffuse irradiance map
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, lightingData.skyboxDiffuseIrradiance);

            //attach skybox specular prefilter map with brdf lut
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, lightingData.skyboxPrefilterMap);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, lightingData.skyboxBrdfLUT);
        }

        updateShaderParams(lightingData, lightingSettings);
        
        // draw the render quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void SoglLightingModule::forwardShading(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, GLuint renderFBO){
        // copy the depth buffer form gbuffer FBO to current FBO for forward pass
        glDisable(GL_CULL_FACE);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, lightingData.gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderFBO);
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        for (std::unique_ptr<SoglGameObject> &gameObj : gameObjects){
            SoglMeshObject* meshObj = static_cast<SoglMeshObject*>(gameObj.get());
            if (meshObj->material.transparent){
                meshObj->drawTransparent(lightingData, lightingSettings, directionalLight);
            }
        }
        
        glEnable(GL_CULL_FACE);
    }


    GLuint SoglLightingModule::getOutputTexture(){
        return outputTex;
    }

#pragma region updateLightingProgram

    void SoglLightingModule::updateShaderParams(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings){
        SoglProgramManager::setMat4("camera.invView", lightingData.camData.invViewMatrix);
        SoglProgramManager::setMat4("camera.invProjection", lightingData.camData.invProjectionMatrix);
        SoglProgramManager::setVec3("camera.position", lightingData.camData.camPos);
        SoglProgramManager::setFloat("camera.near", lightingData.camData.near);
        SoglProgramManager::setFloat("camera.far", lightingData.camData.far);

        if (lightingSettings.shadowEnabled) SoglProgramManager::setMat4("dirLight.transformMatrix", lightingData.dirLightMatrix);

        SoglProgramManager::setVec3("dirLight.color", directionalLight.color);
        SoglProgramManager::setVec3("dirLight.direction", glm::normalize(directionalLight.direction));
        SoglProgramManager::setFloat("dirLight.strength", directionalLight.strength);
    }

    // updates directional light
    void SoglLightingModule::setDirectionalLight(DirectionalLight &dirLight){
        directionalLight = dirLight;
    }

    // recompiles lighting and transparency shader with updated flags (ssao, shadow, etc..) binds the sampler locations to respective indices
    void SoglLightingModule::updateLighting(SoglLightingSettings &lightingSettings){
        std::string lightingParams = "";

        if (lightingSettings.shadowEnabled) lightingParams += "SHADOW_ENABLED,";
        if (lightingSettings.ssaoEnabled) lightingParams += "SSAO_ENABLED,";
        if (lightingSettings.irradianceEnabled) lightingParams += "IRRADIANCE_ENABLED,";

        if (lightingParams.size() > 0) lightingParams = lightingParams.substr(0, lightingParams.size()-1);

        SoglProgramManager::recompileProgram(quadShader, lightingShader, lightingParams);
        SoglProgramManager::useProgram(lightingShader);
        SoglProgramManager::bindImage("gbuffer.gDepth", 0);
        SoglProgramManager::bindImage("gbuffer.gNormalMet", 1);
        SoglProgramManager::bindImage("gbuffer.gAlbedoSpec", 2);
        SoglProgramManager::bindImage("dirLight.shadowMap", 3);
        SoglProgramManager::bindImage("ssaoMap", 4);
        SoglProgramManager::bindImage("skyIrradiance", 5);
        SoglProgramManager::bindImage("prefilterMap", 6);
        SoglProgramManager::bindImage("brdfLUT", 7);
    }
#pragma endregion updateLightingProgram

} // namespace sogl
