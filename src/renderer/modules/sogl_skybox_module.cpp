
#include "renderer/modules/sogl_skybox_module.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace sogl
{
    SoglSkyboxModule::SoglSkyboxModule(const int _WIDTH, const int _HEIGHT): WIDTH{_WIDTH}, HEIGHT{_HEIGHT}{}
    SoglSkyboxModule::~SoglSkyboxModule(){
        glDeleteFramebuffers(1, &captureFBO);
        glDeleteRenderbuffers(1, &captureRBO);

        glDeleteTextures(1, &hdrTexture);
        glDeleteTextures(1, &envCubemap);
        glDeleteTextures(1, &diffuseIrradianceMap);
        glDeleteTextures(1, &prefilterMap);
        glDeleteTextures(1, &brdfLUTTexture);
    }

    void SoglSkyboxModule::loadHDR(std::string hdriPath){
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float *data = stbi_loadf((HDRI_ROOT + hdriPath).c_str(), &width, &height, &nrComponents, 0);

        if (data){
            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); 

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else{
            std::cout << "Failed to load HDR image." << std::endl;
        }
    }

#pragma region skyboxInitialisation
    void SoglSkyboxModule::initialiseSkybox(GLuint cubeVAO, GLuint quadVAO){
        // generate capture frameBuffer
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, ENV_RESOLUTION, ENV_RESOLUTION);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);


        // generate enviornment cubemap
        glGenTextures(1, &envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for (unsigned int i = 0; i < 6; ++i){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, ENV_RESOLUTION, ENV_RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // render settings for all skybox related cubemaps
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        // create shader for converting equilateral hdri to cubemap
        SoglProgramManager::addProgram(cubeShader, skyCubeGenShader, "");
        SoglProgramManager::useProgram(skyCubeGenShader);
        SoglProgramManager::bindImage("equirectangularMap", 0);
        SoglProgramManager::setMat4("projection", captureProjection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        // render equirectangular skybox to cubemap
        glViewport(0, 0, ENV_RESOLUTION, ENV_RESOLUTION);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        glDisable(GL_CULL_FACE); // disable cull face temporarily because the cube is renderer from inside

        for (unsigned int i = 0; i < 6; ++i){
            SoglProgramManager::setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // render cube
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_CULL_FACE);

        // generate environment cubemap mipmaps
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        // initialise the skybox shader for later rendering the skybox
        SoglProgramManager::addProgram(cubeShader, skyRenderShader, "");
        SoglProgramManager::useProgram(skyRenderShader);
        SoglProgramManager::bindImage("skybox", 0);

        initialiseDiffuseIrradiance(captureProjection, captureViews, cubeVAO);
        initialiseSpecularIrradiance(captureProjection, captureViews, cubeVAO, quadVAO);
    }

    void SoglSkyboxModule::initialiseDiffuseIrradiance(glm::mat4 captureProjection, glm::mat4 captureViews[], GLuint cubeVAO){
        // initialise diffuse irradiance cubemap
        glGenTextures(1, &diffuseIrradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, diffuseIrradianceMap);

        for (unsigned int i = 0; i < 6; ++i){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, IRRADIANCE_RESOLUTION, IRRADIANCE_RESOLUTION,
                        0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // initialisation complete----------------------

        // bind framebuffers and prepare for convoluting diffuse irradiance map
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, IRRADIANCE_RESOLUTION, IRRADIANCE_RESOLUTION);

        SoglProgramManager::addProgram(cubeShader, irradianceShader, "");
        SoglProgramManager::useProgram(irradianceShader);
        SoglProgramManager::bindImage("envMap", 0);
        SoglProgramManager::setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        glViewport(0, 0, IRRADIANCE_RESOLUTION, IRRADIANCE_RESOLUTION);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glDisable(GL_CULL_FACE); // disable cull face temporarily because the cube is renderer from inside

        for (unsigned int i = 0; i < 6; ++i){
            SoglProgramManager::setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuseIrradianceMap, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // render cube
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glEnable(GL_CULL_FACE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SoglSkyboxModule::initialiseSpecularIrradiance(glm::mat4 captureProjection, glm::mat4 captureViews[], GLuint cubeVAO, GLuint quadVAO){
        generatePrefilterEnvMap(captureProjection, captureViews, cubeVAO);
        generateBRDFLUT(quadVAO);
    }

    void SoglSkyboxModule::generatePrefilterEnvMap(glm::mat4 captureProjection, glm::mat4 captureViews[], GLuint cubeVAO){
        // generate the prefilter cubemap
        glGenTextures(1, &prefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

        for (unsigned int i = 0; i < 6; ++i){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PREFILTER_RESOLUTION, PREFILTER_RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        // prefilter cubemap initilisation over------------

        // pass in the necessary inputs to the prefilter shader
        SoglProgramManager::addProgram(cubeShader, prefilterShader, "ENV_MAP_RESOLUTION "+std::to_string(float(PREFILTER_RESOLUTION)));
        SoglProgramManager::useProgram(prefilterShader);
        SoglProgramManager::bindImage("envMap", 0);
        SoglProgramManager::setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        // bind the frambuffer and start rendering onto the cubemap
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glDisable(GL_CULL_FACE); // disable cull face temporarily because the cube is renderer from inside
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; mip++){
            // resize framebuffer according to mip-level size.
            unsigned int mipWidth  = static_cast<unsigned int>(PREFILTER_RESOLUTION * std::pow(0.5, mip));
            unsigned int mipHeight = static_cast<unsigned int>(PREFILTER_RESOLUTION * std::pow(0.5, mip));
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            SoglProgramManager::setFloat("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i)
            {
                SoglProgramManager::setMat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // render the cube
                glBindVertexArray(cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        glEnable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SoglSkyboxModule::generateBRDFLUT(GLuint quadVAO){
        // initialise the lut texture
        glGenTextures(1, &brdfLUTTexture);

        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION, 0, GL_RG, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

        SoglProgramManager::addProgram(quadShader, brdfLUTShader, "");
        SoglProgramManager::useProgram(brdfLUTShader);
        glViewport(0, 0, BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw the render quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
#pragma endregion skyboxInitialisation


    void SoglSkyboxModule::renderSkybox(CameraData &camData, GLuint cubeVAO){
        glViewport(0, 0, WIDTH, HEIGHT);
        

        SoglProgramManager::useProgram(skyRenderShader);
        glDisable(GL_CULL_FACE);
        //glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        //pass in the camera uniforms
        SoglProgramManager::setMat4("projection", camData.projectionMatrix);
        SoglProgramManager::setMat4("view", glm::mat4(glm::mat3(camData.viewMatrix)));

        // render cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
    }


#pragma region getters
    GLuint SoglSkyboxModule::getDiffuseIrradiance(){
        return diffuseIrradianceMap;
    }

    GLuint SoglSkyboxModule::getPrefilterMap(){
        return prefilterMap;
    }

    GLuint SoglSkyboxModule::getBrdfLUT(){
        return brdfLUTTexture;
    }
#pragma endregion getters

} // namespace sogl