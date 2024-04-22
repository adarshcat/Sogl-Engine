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

        SoglSkyboxModule(const SoglSkyboxModule&) = delete; // delete copy constructors
        SoglSkyboxModule operator=(const SoglSkyboxModule&) = delete;

        void initialiseSkybox();
        void loadHDR(std::string hdriPath);

        void renderSkybox(CameraData &camData);
        void renderCube();

        const int RESOLUTION = 512;
        const std::string HDRI_ROOT = "assets/hdri/";
        const int WIDTH, HEIGHT;
        
        GLuint envCubemap;
        GLuint hdrTexture;

        private:
        const std::string cubemapShader = "skybox/cubemapShader";
        const std::string skyboxShader = "skybox/skyboxShader";

        
        GLuint captureFBO, captureRBO;

        GLuint cubeVAO = 0;
        GLuint cubeVBO = 0;

    };
} // namespace sogl
