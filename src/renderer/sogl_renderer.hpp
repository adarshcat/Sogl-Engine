#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/sogl_window.hpp"
#include "engine/sogl_camera.hpp"
#include "engine/sogl_game_object.hpp"
#include "util/shaders/sogl_program_manager.hpp"
#include "engine/sogl_lights.hpp"

// std
#include <vector>

namespace sogl
{
    
    class SoglRenderer{
        public:
        SoglRenderer(SoglWindow& wind, const int width, const int height);

        void initialiseLighting(DirectionalLight dirLight);
        bool draw(std::vector<SoglGameObject> &gameObjects, CameraData camData, DirectionalLight dirLight);

        const int WIDTH;
        const int HEIGHT;

        private:
        SoglWindow& soglWindow;
        std::string lightingShader;

        GLuint gBuffer;
        GLuint gPositionView, gNormal, gAlbedoSpec;

        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
        GLuint shadowBuffer;
        GLuint shadowMap;

        GLuint renderQuadVAO;

        void initialiseGBuffer();
        void initialiseRenderQuad();
        void initialiseShadowMap();

        void geometryPass(std::vector<SoglGameObject> &gameObjects, CameraData camData);
        void shadowPass(std::vector<SoglGameObject> &gameObjects, glm::mat4 lightSpaceMatrix);
        void lightingPass(CameraData camData, glm::mat4 lightSpaceMatrix);

        void updateDirectionalLight(DirectionalLight dirLight);
    };
} // namespace sogl

