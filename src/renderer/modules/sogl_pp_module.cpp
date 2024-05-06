
#include "renderer/modules/sogl_pp_module.hpp"

//std
#include <iostream>

namespace sogl
{
    
    SoglPPModule::SoglPPModule(const int _width, const int _height): WIDTH{_width}, HEIGHT{_height},
        INV_WIDTH{(float) (1.0/_width)}, INV_HEIGHT{(float) (1.0/_height)}
    {}

    void SoglPPModule::initialise(){
        SoglProgramManager::addProgram(quadShader, ppShader, "");
        SoglProgramManager::useProgram(ppShader);
        SoglProgramManager::bindImage("hdrImage", 0);
        SoglProgramManager::bindImage("bloomImage", 1);
        SoglProgramManager::setVec2("inverseScreenSize", glm::vec2(INV_WIDTH, INV_HEIGHT));
    }

    void SoglPPModule::render(GLuint hdrOutput, GLuint bloomOutput, GLuint quadVAO){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SoglProgramManager::useProgram(ppShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrOutput);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bloomOutput);

        // draw the render quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
} // namespace sogl
