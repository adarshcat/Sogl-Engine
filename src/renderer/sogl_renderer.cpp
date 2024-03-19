
#include "renderer/sogl_renderer.hpp"

// std
#include <iostream>

namespace sogl
{
    // Setups renderin onto a glfw window
    SoglRenderer::SoglRenderer(SoglWindow& wind, const int width, const int height):
        WIDTH{width}, HEIGHT{height}, soglWindow{wind}
    {
        glewExperimental =  GL_TRUE;

        if (soglWindow.initialiseWindow() == false){
            std::cout << "Failed to initialise the window" << std::endl;
            exit(EXIT_FAILURE);
        } else std::cout << "Window initialised" << std::endl;

        if (glewInit() != GLEW_OK) {
            std::cout << "Failed to initialise GLEW" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "App initialised" << std::endl;
    }

    SoglRenderer::~SoglRenderer(){
        // Delete the G-Buffer
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPositionView);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gAlbedoSpec);
        glDeleteRenderbuffers(1, &gDepth);

        // Delete the render quad
        glDeleteVertexArrays(1, &renderQuadVAO);
        glDeleteBuffers(1, &quadVertexBuffer);

        // Delete directional shadow map buffers
        glDeleteFramebuffers(1, &shadowBuffer);
        glDeleteTextures(1, &shadowMap);
    }

    void SoglRenderer::updateLighting(){
        std::string lightingParams = "";

        if (shadowEnabled) lightingParams += "SHADOW_ENABLED ";
        if (lightingParams.size() > 0) lightingParams = lightingParams.substr(0, lightingParams.size()-1);

        SoglProgramManager::recompileProgram(lightingShader, lightingParams);
        SoglProgramManager::useProgram(lightingShader);
        SoglProgramManager::bindImage("gPositionView", 0);
        SoglProgramManager::bindImage("gNormal", 1);
        SoglProgramManager::bindImage("gAlbedoSpec", 2);
        SoglProgramManager::bindImage("dirLight.shadowMap", 3);
    }

    void SoglRenderer::initialiseRenderer(){
        updateLighting();
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        initialiseGBuffer();
        initialiseRenderQuad();
        initialiseShadowMap();
    }

    void SoglRenderer::initialiseGBuffer(){
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        
        // - position color buffer
        glGenTextures(1, &gPositionView);
        glBindTexture(GL_TEXTURE_2D, gPositionView);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionView, 0);
        
        // - normal color buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        
        // - albedo + specular color buffer
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

        GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        assert(err == GL_FRAMEBUFFER_COMPLETE);
        
        // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // attach basic depth buffer
        glGenRenderbuffers(1, &gDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SoglRenderer::initialiseRenderQuad(){
        glGenVertexArrays(1, &renderQuadVAO);
        glGenBuffers(1, &quadVertexBuffer);

        glBindVertexArray(renderQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);

        const GLfloat quad[] = {
            -1.0, -1.0, 0.0,
             1.0, -1.0, 0.0,
            -1.0,  1.0, 0.0,

            -1.0,  1.0, 0.0,
             1.0, -1.0, 0.0,
             1.0,  1.0, 0.0
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBindVertexArray(0);
    }

    void SoglRenderer::initialiseShadowMap(){
        glGenFramebuffers(1, &shadowBuffer);
        glGenTextures(1, &shadowMap);

        glBindTexture(GL_TEXTURE_2D, shadowMap);

        // shadow texture settings
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 
                    SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

        glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    // Renders onto glfw window, takes in all the renderable game objects and calls draw() on them
    void SoglRenderer::draw(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, CameraData camData, DirectionalLight dirLight){
        glm::mat4 dirLightMatrix = LightOperations::adjustShadowMap(dirLight, camData.frustumSlice1);//dirLight.projectionMatrix * dirLight.viewMatrix;

        glEnable(GL_DEPTH_TEST);
        geometryPass(gameObjects, camData);
        shadowPass(gameObjects, dirLightMatrix);

        glDisable(GL_DEPTH_TEST);
        lightingPass(camData, dirLightMatrix);
    }

    void SoglRenderer::geometryPass(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, CameraData &camData){
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw calls to all the game objects
        for (std::unique_ptr<SoglGameObject> &gameObj : gameObjects){
            gameObj->draw(camData);
        }
    }

    void SoglRenderer::shadowPass(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, glm::mat4 &dirLightMatrix){
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        //glCullFace(GL_FRONT);
        // Draw shadow calls to all the game objects
        for (std::unique_ptr<SoglGameObject> &gameObj : gameObjects){
            gameObj->drawShadow(dirLightMatrix);
        }
        //glCullFace(GL_BACK);

        // reset stuff
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WIDTH, HEIGHT);
    }

    void SoglRenderer::lightingPass(CameraData &camData, glm::mat4 &dirLightMatrix){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SoglProgramManager::useProgram(lightingShader);

        // bind the g-buffer to lighting shader program
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPositionView);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, shadowMap);

        SoglProgramManager::setMat4("invViewMatrix", camData.invViewMatrix);
        SoglProgramManager::setVec3("cameraPos", camData.camPos);

        SoglProgramManager::setMat4("dirLight.transformMatrix", dirLightMatrix);
        
        // draw the render quad
        glBindVertexArray(renderQuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    void SoglRenderer::updateDirectionalLight(DirectionalLight &dirLight){
        SoglProgramManager::useProgram(lightingShader);
        SoglProgramManager::setVec3("dirLight.color", dirLight.color);
        SoglProgramManager::setVec3("dirLight.direction", glm::normalize(dirLight.direction));
        SoglProgramManager::setFloat("dirLight.strength", dirLight.strength);
    }

    void SoglRenderer::toggleShadows(const bool state, DirectionalLight &dirLight){
        if (shadowEnabled == state) return;

        shadowEnabled = state;
        updateLighting();
        updateDirectionalLight(dirLight);
    }
} // namespace sogl
