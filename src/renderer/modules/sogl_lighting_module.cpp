
#include "renderer/modules/sogl_lighting_module.hpp"

namespace sogl
{
    
    SoglLightingModule::SoglLightingModule(const int _width, const int _height): WIDTH{_width}, HEIGHT{_height}{}
    SoglLightingModule::~SoglLightingModule(){

    }

    void SoglLightingModule::initialiseLighting(){
        // generate the frame buffer
        glGenFramebuffers(1, &FBO);  
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // generate the output texture
        glGenTextures(1, &outputTex);
        glBindTexture(GL_TEXTURE_2D, outputTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        SoglLightingSettings ls;
        updateLighting(ls);
    }


    void SoglLightingModule::lightingPass(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, GLuint quadVAO){
        glViewport(0, 0, WIDTH, HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

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
            SoglProgramManager::setMat4("dirLight.transformMatrix", lightingData.dirLightMatrix);
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

        // pass necessary camera parameters
        SoglProgramManager::setMat4("camera.invView", lightingData.camData.invViewMatrix);
        SoglProgramManager::setMat4("camera.invProjection", lightingData.camData.invProjectionMatrix);
        SoglProgramManager::setVec3("camera.position", lightingData.camData.camPos);
        SoglProgramManager::setFloat("camera.near", lightingData.camData.near);
        SoglProgramManager::setFloat("camera.far", lightingData.camData.far);
        
        // draw the render quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint SoglLightingModule::getOutputTexture(){
        return outputTex;
    }

    GLuint SoglLightingModule::getFBO(){
        return FBO;
    }


#pragma region updateLightingProgram

    // updates directional light + etc. parameters in the shader
    void SoglLightingModule::updateLightingShaderInputs(DirectionalLight &dirLight){
        directionalLight = dirLight;

        SoglProgramManager::useProgram(lightingShader);
        SoglProgramManager::setVec3("dirLight.color", directionalLight.color);
        SoglProgramManager::setVec3("dirLight.direction", glm::normalize(directionalLight.direction));
        SoglProgramManager::setFloat("dirLight.strength", directionalLight.strength);
    }

    // recompiles lighting shader with updated flags (ssao, shadow, etc..) binds the sampler locations to respective indices
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

        updateLightingShaderInputs(directionalLight);
    }
#pragma endregion updateLightingProgram

} // namespace sogl
