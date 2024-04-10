#include "sogl_ssao_module.hpp"

namespace sogl
{
    SoglSSAOModule::SoglSSAOModule(const unsigned int _WIDTH, const unsigned int _HEIGHT, const unsigned int _SAMPLES):
        SSAO_WIDTH{_WIDTH}, SSAO_HEIGHT{_HEIGHT}, SSAO_SAMPLES{_SAMPLES}
    {

    }

    SoglSSAOModule::~SoglSSAOModule(){
        // Delete SSAO related stuff
        glDeleteFramebuffers(1, &ssaoFBO);
        glDeleteTextures(1, &ssaoNoiseTex);
        glDeleteTextures(1, &ssaoOutput);

        // Delete SSAO blur stuff
        glDeleteFramebuffers(1, &ssaoBlurFBO);
        glDeleteTextures(1, &ssaoBlurOutput);
    }


#pragma region initialisation
    void SoglSSAOModule::initialiseSSAO(){
        // generate the SSAO kernel
        std::uniform_real_distribution<float> randomFloats(0, 1);
        std::default_random_engine randomGenerator;

        for (unsigned int i=0; i<SSAO_SAMPLES; i++){
            glm::vec3 sample(
                randomFloats(randomGenerator) * 2.0 - 1.0, 
                randomFloats(randomGenerator) * 2.0 - 1.0, 
                randomFloats(randomGenerator)
            );

            sample = glm::normalize(sample);
            sample *= randomFloats(randomGenerator);

            float scale = ((float)i) / SSAO_SAMPLES;
            scale = sogl::Helper::lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }

        // generate ssao noise samples
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++){
            glm::vec3 noise(
                randomFloats(randomGenerator) * 2.0 - 1.0,
                randomFloats(randomGenerator) * 2.0 - 1.0,
                0.0f);
            ssaoNoise.push_back(noise);
        }

        // pack ssao noise samples into a repeating ssao noise texture
        glGenTextures(1, &ssaoNoiseTex);
        glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // create framebuffer for ssao rendering, attach a output texture
        glGenFramebuffers(1, &ssaoFBO);  
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

        glGenTextures(1, &ssaoOutput);
        glBindTexture(GL_TEXTURE_2D, ssaoOutput);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SSAO_WIDTH, SSAO_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoOutput, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // initialise the ssao shader program
        const std::string ssaoParams = "KERNEL_SIZE "+std::to_string(SSAO_SAMPLES)+
                                        ",WINDOW_WIDTH "+std::to_string(SSAO_WIDTH)+",WINDOW_HEIGHT "+std::to_string(SSAO_HEIGHT);
        SoglProgramManager::addProgram(ssaoShader, ssaoParams);
        SoglProgramManager::useProgram(ssaoShader);

        SoglProgramManager::bindImage("gDepth", 0);
        SoglProgramManager::bindImage("gNormal", 1);
        SoglProgramManager::bindImage("noiseTexture", 2);

        // send the kernel over to the SSAO shader program
        for (unsigned int i = 0; i < SSAO_SAMPLES; i++)
            SoglProgramManager::setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    }

    void SoglSSAOModule::initialiseSSAOBlur(){
        glGenFramebuffers(1, &ssaoBlurFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glGenTextures(1, &ssaoBlurOutput);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurOutput);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SSAO_WIDTH, SSAO_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurOutput, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        SoglProgramManager::addProgram(ssaoBlurShader);
        SoglProgramManager::useProgram(ssaoBlurShader);

        SoglProgramManager::bindImage("depthBuffer", 0);
        SoglProgramManager::bindImage("ssaoInput", 1);
    }
#pragma endregion initialisation


    void SoglSSAOModule::ssaoPass(CameraData &camData, GLuint gDepth, GLuint gNormal, GLuint renderQuadVAO, bool ssaoBlurEnabled){
        glViewport(0, 0, SSAO_WIDTH, SSAO_HEIGHT);
        // ssao pass
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        SoglProgramManager::useProgram(ssaoShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gDepth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ssaoNoiseTex);

        SoglProgramManager::setMat4("viewMatrix", camData.viewMatrix);
        SoglProgramManager::setMat4("projectionMatrix", camData.projectionMatrix);
        SoglProgramManager::setMat4("invProjection", camData.invProjectionMatrix);

        // draw the render quad
        glBindVertexArray(renderQuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Do SSAO blur
        if (ssaoBlurEnabled){
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);

            SoglProgramManager::useProgram(ssaoBlurShader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gDepth);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, ssaoOutput);

            SoglProgramManager::setFloat("near", camData.near);
            SoglProgramManager::setFloat("far", camData.far);

            // draw the render quad
            glBindVertexArray(renderQuadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // namespace sogl
