#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "util/shaders/sogl_program_manager.hpp"
#include "engine/env/sogl_camera.hpp"

//std
#include <string>
#include <iostream>

namespace sogl
{
    
    class SoglSkyboxModule{
        public:
        SoglSkyboxModule(const int _WIDTH, const int _HEIGHT);
        ~SoglSkyboxModule();

        // delete copy constructors
        SoglSkyboxModule(const SoglSkyboxModule&) = delete; 
        SoglSkyboxModule operator=(const SoglSkyboxModule&) = delete;
        // --------------------------

        void loadHDR(std::string hdriPath);
        void initialiseSkybox(GLuint cubeVAO);

        GLuint getDiffuseIrradiance();
        void renderSkybox(CameraData &camData, GLuint cubeVAO);

        const int ENV_RESOLUTION = 512*2;
        const int IRRADIANCE_RESOLUTION = 32;
        const std::string HDRI_ROOT = "assets/hdri/";
        const int WIDTH, HEIGHT;

        private:
        const std::string cubemapShader = "skybox/cubemapShader";
        const std::string skyboxShader = "skybox/skyboxShader";
        const std::string irradianceShader = "skybox/irradianceShader";

        void initialiseDiffuseIrradiance(glm::mat4 captureProjection, glm::mat4 captureViews[], GLuint cubeVAO);
        
        GLuint envCubemap, diffuseIrradianceMap;
        GLuint hdrTexture;
        GLuint captureFBO, captureRBO;

    };
} // namespace sogl
