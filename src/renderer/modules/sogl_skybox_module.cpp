
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

    void SoglSkyboxModule::initialiseSkybox(GLuint cubeVAO){
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
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // render settings for cubemap
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

        // create shader for converting to cubemap
        SoglProgramManager::addProgram(cubemapShader);
        SoglProgramManager::useProgram(cubemapShader);
        SoglProgramManager::bindImage("equirectangularMap", 0);
        SoglProgramManager::setMat4("projection", captureProjection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        // render equirectangular skybox to cubemap
        glViewport(0, 0, ENV_RESOLUTION, ENV_RESOLUTION);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        glDisable(GL_CULL_FACE);

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

        // initialise the skybox shader for later rendering the skybox
        SoglProgramManager::addProgram(skyboxShader);
        SoglProgramManager::useProgram(skyboxShader);
        SoglProgramManager::bindImage("skybox", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        initialiseDiffuseIrradiance(captureProjection, captureViews, cubeVAO);
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

        SoglProgramManager::addProgram(irradianceShader);
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


    void SoglSkyboxModule::renderSkybox(CameraData &camData, GLuint cubeVAO){
        glViewport(0, 0, WIDTH, HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        SoglProgramManager::useProgram(skyboxShader);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        //pass in the camera uniforms
        SoglProgramManager::setMat4("projection", camData.projectionMatrix);
        SoglProgramManager::setMat4("view", glm::mat4(glm::mat3(camData.viewMatrix)));

        // render cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
    }


#pragma region getters
    GLuint SoglSkyboxModule::getDiffuseIrradiance(){
        return diffuseIrradianceMap;
    }
#pragma endregion getters

} // namespace sogl