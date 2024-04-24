#pragma once

#include <GL/glew.h>
#include "util/shaders/sogl_program_manager.hpp"

// std
#include <string>

namespace sogl
{
    
    class SoglPPModule{
        public:
        SoglPPModule(const int _width, const int _height);

        void initialise();

        void render(GLuint hdrOutput, GLuint quadVAO);

        private:
        const int WIDTH = 400, HEIGHT = 500;
        const std::string quadShader = "render_quad";
        const std::string ppShader = "postprocessing/pp";
    };
} // namespace sogl
