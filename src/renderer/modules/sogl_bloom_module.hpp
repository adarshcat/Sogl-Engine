#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "util/shaders/sogl_program_manager.hpp"

//std
#include <string>
#include <vector>

namespace sogl
{
    struct BloomMip{
        glm::vec2 size;
        glm::ivec2 intSize;
        GLuint texture;
    };
    
    class SoglBloomModule{
        public:
        SoglBloomModule(const int _width, const int _height);
        ~SoglBloomModule();

        void initialise();
        void bloomPass(GLuint srcTexture, GLuint quadVAO);
        GLuint getBloomOutput();

        std::vector<BloomMip> mipChain;

        private:
        const int WIDTH, HEIGHT;
        const int MIP_CHAIN_LENGTH = 5;

        const std::string quadShader = "render_quad";
        const std::string upsampleShader = "bloom/upsample";
        const std::string downsampleShader = "bloom/downsample";

        void renderDownsamples(GLuint srcTexture, GLuint quadVAO);
        void renderUpsamples(GLuint quadVAO);

        GLuint renderFBO;
    };
} // namespace sogl
