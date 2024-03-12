#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "sogl_window.hpp"
#include "sogl_camera.hpp"
#include "sogl_game_object.hpp"
#include "sogl_program_manager.hpp"

// std
#include <vector>

namespace sogl
{
    
    class SoglRenderer{
        public:
        SoglRenderer(SoglWindow& wind, const int width, const int height);
        void initialiseRenderer();
        void initialiseGBuffer();
        void initialiseRenderQuad();

        bool draw(std::vector<SoglGameObject> &gameObjects, glm::mat4 viewProjectionMatrix, glm::vec3 camPos);

        const int WIDTH;
        const int HEIGHT;

        private:
        SoglWindow& soglWindow;
        std::string lightingShader;

        GLuint gBuffer;
        GLuint gPosition, gNormal, gAlbedoSpec;

        GLuint renderQuadVAO;

        void geometryPass(std::vector<SoglGameObject> &gameObjects, glm::mat4 viewProjectionMatrix, glm::vec3 camPos);
        void lightingPass();
        
    };
} // namespace sogl

