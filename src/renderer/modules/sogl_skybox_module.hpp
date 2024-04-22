#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "util/shaders/sogl_program_manager.hpp"

//std
#include <string>
#include <iostream>

namespace sogl
{
    
    class SoglSkyboxModule{
        public:
        SoglSkyboxModule(){};
        ~SoglSkyboxModule();

        SoglSkyboxModule(const SoglSkyboxModule&) = delete; // delete copy constructors
        SoglSkyboxModule operator=(const SoglSkyboxModule&) = delete;

        void initialiseSkybox();
        void loadHDR(std::string hdriPath);

        const int RESOLUTION = 512;
        const std::string HDRI_ROOT = "assets/hdri/";
        
        GLuint envCubemap;
        GLuint hdrTexture;

        private:
        const std::string cubemapShader = "skybox/cubemapShader";

        
        GLuint captureFBO, captureRBO;

        GLuint cubeVAO = 0;
        GLuint cubeVBO = 0;
        void renderCube();

    };
} // namespace sogl
