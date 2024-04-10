#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/sogl_camera.hpp"
#include "util/sogl_helper.hpp"
#include "util/shaders/sogl_program_manager.hpp"

// std
#include <iostream>
#include <vector>
#include <memory>
#include <random>

namespace sogl
{
    
    class SoglSSAOModule{
        public:
        SoglSSAOModule(const unsigned int _WIDTH, const unsigned int _HEIGHT, const unsigned int _SAMPLES);
        ~SoglSSAOModule();

        void initialiseSSAO();
        void initialiseSSAOBlur();

        void ssaoPass(CameraData &camData, GLuint gDepth, GLuint gNormal, GLuint renderQuadVAO, bool ssaoBlurEnabled);

        GLuint ssaoBlurOutput, ssaoOutput;

        private:
        std::string ssaoShader = "ssao";
        std::string ssaoBlurShader = "ssao_blur";

        //ssao
        const unsigned int SSAO_WIDTH;
        const unsigned int SSAO_HEIGHT;
        const unsigned int SSAO_SAMPLES = 32;

        std::vector<glm::vec3> ssaoKernel;
        GLuint ssaoFBO;
        GLuint ssaoNoiseTex;

        //ssao blur
        GLuint ssaoBlurFBO;
        
    };
} // namespace sogl
