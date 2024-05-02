
#include "sogl_bloom_module.hpp"

namespace sogl
{
    
    SoglBloomModule::SoglBloomModule(const int _width, const int _height): WIDTH{_width}, HEIGHT{_height}
    {

    }

    SoglBloomModule::~SoglBloomModule(){
        for (int i = 0; i < mipChain.size(); i++) {
            glDeleteTextures(1, &mipChain[i].texture);
            mipChain[i].texture = 0;
        }
        glDeleteFramebuffers(1, &renderFBO);
        renderFBO = 0;
    }

    void SoglBloomModule::initialise(){
        // initialise frame buffer
        glGenFramebuffers(1, &renderFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

        glm::vec2 mipSize((float)WIDTH, (float)HEIGHT);
        glm::ivec2 mipIntSize((int)WIDTH, (int)HEIGHT);

        // initialise all the textures in the mip chain
        for (unsigned int i = 0; i < MIP_CHAIN_LENGTH; i++){
            BloomMip mip;
            mipSize *= 0.5f;
            mipIntSize /= 2;

            mip.size = mipSize;
            mip.intSize = mipIntSize;

            glGenTextures(1, &mip.texture);
            glBindTexture(GL_TEXTURE_2D, mip.texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            mipChain.emplace_back(mip);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mipChain[0].texture, 0);

        GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // initialise bloom shaders
        SoglProgramManager::addProgram(quadShader, upsampleShader, "");
        SoglProgramManager::useProgram(upsampleShader);
        SoglProgramManager::bindImage("srcTexture", 0);
        SoglProgramManager::addProgram(quadShader, downsampleShader, "");
        SoglProgramManager::useProgram(downsampleShader);
        SoglProgramManager::bindImage("srcTexture", 0);
    }


    GLuint SoglBloomModule::getBloomOutput(){
        return mipChain[0].texture;
    }

    void SoglBloomModule::bloomPass(GLuint srcTexture, GLuint quadVAO){
        glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
        renderDownsamples(srcTexture, quadVAO);
        renderUpsamples(quadVAO);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WIDTH, HEIGHT);
    }

    void SoglBloomModule::renderDownsamples(GLuint srcTexture, GLuint quadVAO){
        SoglProgramManager::useProgram(downsampleShader);
        SoglProgramManager::setVec2("srcResolution", glm::vec2(WIDTH, HEIGHT));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcTexture);

        for (int i = 0; i < mipChain.size(); i++){
            const BloomMip& mip = mipChain[i];
            glViewport(0, 0, mip.size.x, mip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);
            SoglProgramManager::setFloat("mipLevel", i);

            // render quad
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            // set new resolution and bind the next texture for downsample
            SoglProgramManager::setVec2("srcResolution", mip.size);
            glBindTexture(GL_TEXTURE_2D, mip.texture);
        }
    }

    void SoglBloomModule::renderUpsamples(GLuint quadVAO){
        SoglProgramManager::useProgram(upsampleShader);
        // SoglProgramManager::setFloat("filterRadius", filterRadius);

        // enable additive blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        for (int i = mipChain.size() - 1; i > 0; i--){
            const BloomMip& mip = mipChain[i];
            const BloomMip& nextMip = mipChain[i-1];

            // set the smaller mip as the source texture to read from
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mip.texture);

            // set framebuffer render target to bigger mip
            glViewport(0, 0, nextMip.size.x, nextMip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);

            // render quad
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        glDisable(GL_BLEND);
    }

} // namespace sogl
