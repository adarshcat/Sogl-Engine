#pragma once

#include "util/shaders/sogl_program_manager.hpp"
#include "engine/env/sogl_camera.hpp"
#include "engine/env/sogl_lights.hpp"
#include "engine/game/sogl_game_object.hpp"
#include "engine/game/sogl_mesh_object.hpp"
#include "util/sogl_helper.hpp"

//std
#include <vector>
#include <memory>

namespace sogl
{
    
    struct SoglLightingData{
        CameraData camData;
        glm::mat4 dirLightMatrix;

        GLuint gBuffer;
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

        void initialiseLighting(GLuint renderFBO);

        void lightingPass(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, std::vector<std::unique_ptr<SoglGameObject>> &gameObjects,
                        GLuint renderFBO, GLuint quadVAO);
        void defferedShading(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, GLuint quadVAO);
        void forwardShading(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings, std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, GLuint renderFBO);
        void updateShaderParams(SoglLightingData &lightingData, SoglLightingSettings &lightingSettings);

        GLuint getOutputTexture();

        void updateLighting(SoglLightingSettings &lightingSettings);
        
        void setDirectionalLight(DirectionalLight &dirLight);

        private:
        const int WIDTH, HEIGHT;
        const std::string quadShader = "render_quad";
        const std::string lightingShader = "lighting/pbr";

        //GLuint FBO;
        GLuint outputTex;

        DirectionalLight directionalLight;
    };
} // namespace sogl
