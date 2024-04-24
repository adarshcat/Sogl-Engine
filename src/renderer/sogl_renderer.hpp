#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/sogl_window.hpp"
#include "engine/env/sogl_camera.hpp"
#include "engine/game/sogl_game_object.hpp"
#include "util/shaders/sogl_program_manager.hpp"
#include "engine/env/sogl_lights.hpp"
#include "util/sogl_helper.hpp"

#include "renderer/modules/sogl_lighting_module.hpp"
#include "renderer/modules/sogl_ssao_module.hpp"
#include "renderer/modules/sogl_skybox_module.hpp"
#include "renderer/modules/sogl_pp_module.hpp"

// std
#include <vector>
#include <memory>
#include <random>

namespace sogl
{
    
    class SoglRenderer{
        public:
        SoglRenderer(SoglWindow& wind, const int width, const int height);
        ~SoglRenderer();

        SoglRenderer(const SoglRenderer&) = delete; // delete copy constructors
        SoglRenderer operator=(const SoglRenderer&) = delete;
        
        // render dimensions
        const int WIDTH;
        const int HEIGHT;

        enum settingTypes{
            SHADOWS,
            SSAO,
            SSAO_BLUR,
            IRRADIANCE
        };

        void initialiseRenderer();

        // lighting update functions
        void updateDirectionalLight(DirectionalLight &dirLight);

        void draw(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, CameraData camData);

        // changed renderer settings
        void toggleSetting(settingTypes type, bool state);

        private:
        const std::string skyboxImage = "veranda_4k.hdr";

        SoglWindow& soglWindow;
        DirectionalLight directionalLight;

        // renderer settings
        SoglLightingSettings lightingSettings;

        // renderer initialisation functions
        void initialiseGBuffer();
        void initialiseRenderQuad();
        void initialiseRenderCube();
        void initialiseShadowMap();

        // renderer modules
        SoglLightingModule lightingModule;
        SoglSSAOModule ssaoModule;
        SoglSkyboxModule skyboxModule;
        SoglPPModule ppModule;

        //g-buffer
        GLuint gBuffer;
        GLuint gPositionView, gNormalMet, gAlbedoSpec, gDepth;

        //shadow
        const unsigned int SHADOW_WIDTH = WIDTH*1.25, SHADOW_HEIGHT = WIDTH*1.25;
        GLuint shadowBuffer;
        GLuint shadowMap;

        //render quad
        GLuint quadVertexBuffer;
        GLuint renderQuadVAO;

        //render cube
        GLuint renderCubeVAO;
        GLuint renderCubeVBO;

        // render passes
        void geometryPass(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, CameraData &camData);
        void shadowPass(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, glm::mat4 &lightSpaceMatrix);
        void lightingPass(CameraData &camData, glm::mat4 &lightSpaceMatrix);
    };

} // namespace sogl

