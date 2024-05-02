#pragma once

#include "util/shaders/sogl_program_manager.hpp"
#include "engine/env/sogl_camera.hpp"
#include "engine/env/sogl_lights.hpp"
#include "util/sogl_helper.hpp"

namespace sogl
{
    
    struct SoglLightingData{
        CameraData camData;
        glm::mat4 dirLightMatrix;

        GLuint gDepth;
        GLuint gNormalMet;
        GLuint gAlbedoSpec;

        GLuint shadowMap;

        GLuint ssaoBlurOutput;
        GLuint ssaoOutput;

        GLuint skyboxDiffuseIrradiance;
        GLuint skyboxPrefilterMap;
        GLuint skyboxBrdfLUT;
    };

    struct SoglLightingSettings{
        bool shadowEnabled = true;
        bool ssaoEnabled = true;
        bool ssaoBlurEnabled = true;
        bool irradianceEnabled = true;
    };

    class SoglLightingModule{
        public:
        SoglLightingModule(const int _width, const int _height);
        ~SoglLightingModule();

        void initialiseLighting();

        void lightingPass(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, GLuint quadVAO);
        GLuint getOutputTexture();
        GLuint getFBO();

        void updateLighting(SoglLightingSettings &lightingSettings);
        void updateLightingShaderInputs(DirectionalLight &dirLight);

        private:
        const int WIDTH, HEIGHT;
        const std::string quadShader = "render_quad";
        const std::string lightingShader = "lighting/pbr";

        GLuint FBO;
        GLuint outputTex;

        DirectionalLight directionalLight;
    };
} // namespace sogl
