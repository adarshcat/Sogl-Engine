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
        void initialiseSkybox(GLuint cubeVAO, GLuint quadVAO);

        GLuint getDiffuseIrradiance();
        GLuint getPrefilterMap();
        GLuint getBrdfLUT();
        void renderSkybox(CameraData &camData, GLuint cubeVAO);

        private:
        const int ENV_RESOLUTION = 512;
        const int IRRADIANCE_RESOLUTION = 32;
        const int PREFILTER_RESOLUTION = 128;
        const int BRDF_LUT_RESOLUTION = 512;

        const std::string HDRI_ROOT = "assets/hdri/";
        const int WIDTH, HEIGHT;

        const std::string cubemapShader = "skybox/skybox_gen";
        const std::string skyboxShader = "skybox/skybox_render";
        const std::string irradianceShader = "skybox/diffuse_irradiance";
        const std::string prefilterShader = "skybox/prefilter";
        const std::string brdfLUTShader = "skybox/brdflut_gen";

        void initialiseDiffuseIrradiance(glm::mat4 captureProjection, glm::mat4 captureViews[], GLuint cubeVAO);

        void initialiseSpecularIrradiance(glm::mat4 captureProjection, glm::mat4 captureViews[], GLuint cubeVAO, GLuint quadVAO);
        void generatePrefilterEnvMap(glm::mat4 captureProjection, glm::mat4 captureViews[], GLuint cubeVAO);
        void generateBRDFLUT(GLuint quadVAO);

        GLuint envCubemap, diffuseIrradianceMap;
        GLuint prefilterMap, brdfLUTTexture;
        GLuint hdrTexture;
        GLuint captureFBO, captureRBO;

    };
} // namespace sogl
