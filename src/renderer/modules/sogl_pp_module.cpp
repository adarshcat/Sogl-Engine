
#include "renderer/modules/sogl_pp_module.hpp"

namespace sogl
{
    
    SoglPPModule::SoglPPModule(const int _width, const int _height): WIDTH{_width}, HEIGHT{_height}{}

    void SoglPPModule::initialise(){
        SoglProgramManager::addProgram(quadShader, ppShader, "");
        SoglProgramManager::useProgram(ppShader);
        SoglProgramManager::bindImage("hdrImage", 0);
    }

    void SoglPPModule::render(GLuint hdrOutput, GLuint quadVAO){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        SoglProgramManager::useProgram(ppShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrOutput);

        // draw the render quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
} // namespace sogl
