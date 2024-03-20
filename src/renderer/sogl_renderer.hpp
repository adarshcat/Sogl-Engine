#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/sogl_window.hpp"
#include "engine/sogl_camera.hpp"
#include "engine/sogl_game_object.hpp"
#include "util/shaders/sogl_program_manager.hpp"
#include "engine/sogl_lights.hpp"
#include "util/sogl_helper.hpp"

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

        void initialiseRenderer();
        void updateDirectionalLight(DirectionalLight &dirLight);
        void draw(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, CameraData camData, DirectionalLight dirLight);

        // changed renderer settings
        void toggleShadows(const bool state, DirectionalLight &dirLight);
        void toggleSSAO(const bool state);

        const int WIDTH;
        const int HEIGHT;

        private:
        SoglWindow& soglWindow;
        std::string lightingShader = "lighting";
        std::string ssaoShader = "ssao";

        //g-buffer
        GLuint gBuffer;
        GLuint gPositionView, gNormal, gAlbedoSpec, gDepth;

        //shadow
        const unsigned int SHADOW_WIDTH = 1024*2, SHADOW_HEIGHT = 1024*2;
        GLuint shadowBuffer;
        GLuint shadowMap;

        //ssao
        const unsigned int SSAO_SAMPLES = 12;
        std::vector<glm::vec3> ssaoKernel;
        GLuint ssaoFBO;
        GLuint ssaoNoiseTex, ssaoOutput;

        //render quad
        GLuint quadVertexBuffer;
        GLuint renderQuadVAO;

        // renderer settings
        bool shadowEnabled = true;
        bool ssaoEnabled = true;
        void updateLighting();

        void initialiseGBuffer();
        void initialiseRenderQuad();
        void initialiseShadowMap();
        void initialiseSSAO();

        void geometryPass(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, CameraData &camData);
        void shadowPass(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, glm::mat4 &lightSpaceMatrix);
        void ssaoPass(CameraData &camData);
        void lightingPass(CameraData &camData, glm::mat4 &lightSpaceMatrix);
    };

} // namespace sogl

