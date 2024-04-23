
#include "renderer/sogl_renderer.hpp"

// std
#include <iostream>

namespace sogl
{
    // Setups renderin onto a glfw window
    SoglRenderer::SoglRenderer(SoglWindow& wind, const int width, const int height):
        WIDTH{width}, HEIGHT{height}, soglWindow{wind}, ssaoModule{SoglSSAOModule(width/2, height/2, 32)},
        skyboxModule{SoglSkyboxModule(width, height)}
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
        glDeleteTextures(1, &gNormalMet);
        glDeleteTextures(1, &gAlbedoSpec);
        glDeleteTextures(1, &gDepth);

        // Delete the render quad
        glDeleteVertexArrays(1, &renderQuadVAO);
        glDeleteBuffers(1, &quadVertexBuffer);

        // Delete the render cube
        glDeleteVertexArrays(1, &renderCubeVAO);
        glDeleteBuffers(1, &renderCubeVBO);

        // Delete directional shadow map buffers
        glDeleteFramebuffers(1, &shadowBuffer);
        glDeleteTextures(1, &shadowMap);
    }


#pragma region rendererInitialisation
    void SoglRenderer::initialiseRenderer(){
        updateLighting();
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        initialiseGBuffer();
        initialiseRenderQuad();
        initialiseRenderCube();
        initialiseShadowMap();

        ssaoModule.initialiseSSAO();
        ssaoModule.initialiseSSAOBlur();

        skyboxModule.loadHDR(skyboxImage);
        skyboxModule.initialiseSkybox(renderCubeVAO, renderQuadVAO);
    }

    void SoglRenderer::initialiseGBuffer(){
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        
        // - normal color buffer
        glGenTextures(1, &gNormalMet);
        glBindTexture(GL_TEXTURE_2D, gNormalMet);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormalMet, 0);
        
        // - albedo + specular color buffer
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedoSpec, 0);

        // attach basic depth buffer
        glGenTextures(1, &gDepth);
        glBindTexture(GL_TEXTURE_2D, gDepth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

        // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

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

    void SoglRenderer::initialiseRenderCube(){
        GLfloat vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &renderCubeVAO);
        glGenBuffers(1, &renderCubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, renderCubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(renderCubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void SoglRenderer::initialiseShadowMap(){
        glGenFramebuffers(1, &shadowBuffer);
        glGenTextures(1, &shadowMap);

        glBindTexture(GL_TEXTURE_2D, shadowMap);

        // shadow texture settings
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
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
#pragma endregion rendererInitialisation



    // Renders onto glfw window, takes in all the renderable game objects and calls draw() on them
    void SoglRenderer::draw(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, CameraData camData){
        glm::mat4 dirLightMatrix = LightOperations::adjustShadowMap(directionalLight, camData.frustumSlice1);

        glEnable(GL_DEPTH_TEST);

        geometryPass(gameObjects, camData);
        if (shadowEnabled) shadowPass(gameObjects, dirLightMatrix);

        glDisable(GL_DEPTH_TEST);

        if (ssaoEnabled) ssaoModule.ssaoPass(camData, gDepth, gNormalMet, renderQuadVAO, ssaoBlurEnabled);

        // clear the screen and render the final image
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        skyboxModule.renderSkybox(camData, renderCubeVAO);
        lightingPass(camData, dirLightMatrix);
    }


#pragma region renderpasses
    void SoglRenderer::geometryPass(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, CameraData &camData){
        glViewport(0, 0, WIDTH, HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw calls to all the game objects
        for (std::unique_ptr<SoglGameObject> &gameObj : gameObjects){
            gameObj->draw(camData);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SoglRenderer::shadowPass(std::vector<std::unique_ptr<SoglGameObject>> &gameObjects, glm::mat4 &dirLightMatrix){
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        glCullFace(GL_FRONT);
        // Draw shadow calls to all the game objects
        for (std::unique_ptr<SoglGameObject> &gameObj : gameObjects){
            gameObj->drawShadow(dirLightMatrix);
        }
        glCullFace(GL_BACK);

        // reset stuff
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SoglRenderer::lightingPass(CameraData &camData, glm::mat4 &dirLightMatrix){
        glViewport(0, 0, WIDTH, HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        SoglProgramManager::useProgram(lightingShader);

        // bind the g-buffer to lighting shader program
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gDepth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormalMet);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        if (shadowEnabled){
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, shadowMap);
            SoglProgramManager::setMat4("dirLight.transformMatrix", dirLightMatrix);
        }
        if (ssaoEnabled){
            glActiveTexture(GL_TEXTURE4);
            if (ssaoBlurEnabled)
                glBindTexture(GL_TEXTURE_2D, ssaoModule.ssaoBlurOutput);
            else
                glBindTexture(GL_TEXTURE_2D, ssaoModule.ssaoOutput);
        }

        //attach skybox diffuse irradiance map
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxModule.getDiffuseIrradiance());

        //attach skybox specular prefilter map with brdf lut
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxModule.getPrefilterMap());
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, skyboxModule.getBrdfLUT());

        // pass necessary camera parameters
        SoglProgramManager::setMat4("camera.invView", camData.invViewMatrix);
        SoglProgramManager::setMat4("camera.invProjection", camData.invProjectionMatrix);
        SoglProgramManager::setVec3("camera.position", camData.camPos);
        SoglProgramManager::setFloat("camera.near", camData.near);
        SoglProgramManager::setFloat("camera.far", camData.far);
        
        // draw the render quad
        glBindVertexArray(renderQuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
#pragma endregion renderpasses


#pragma region updateLightingProgram
    // updates directional light parameters and also updates in the shader
    void SoglRenderer::updateDirectionalLight(DirectionalLight &dirLight){
        directionalLight.color = dirLight.color;
        directionalLight.direction = dirLight.direction;
        directionalLight.strength = dirLight.strength;

        updateLightingShaderInputs();
    }

    // updates directional light + etc. parameters in the shader
    void SoglRenderer::updateLightingShaderInputs(){
        SoglProgramManager::useProgram(lightingShader);
        SoglProgramManager::setVec3("dirLight.color", directionalLight.color);
        SoglProgramManager::setVec3("dirLight.direction", glm::normalize(directionalLight.direction));
        SoglProgramManager::setFloat("dirLight.strength", directionalLight.strength);
    }

    // recompiles lighting shader with updated flags (ssao, shadow, etc..) binds the sampler locations to respective indices
    void SoglRenderer::updateLighting(){
        std::string lightingParams = "";

        if (shadowEnabled) lightingParams += "SHADOW_ENABLED,";
        if (ssaoEnabled) lightingParams += "SSAO_ENABLED,";
        if (irradianceEnabled) lightingParams += "IRRADIANCE_ENABLED,";

        if (lightingParams.size() > 0) lightingParams = lightingParams.substr(0, lightingParams.size()-1);

        SoglProgramManager::recompileProgram(lightingShader, lightingParams);
        SoglProgramManager::useProgram(lightingShader);
        SoglProgramManager::bindImage("gbuffer.gDepth", 0);
        SoglProgramManager::bindImage("gbuffer.gNormalMet", 1);
        SoglProgramManager::bindImage("gbuffer.gAlbedoSpec", 2);
        SoglProgramManager::bindImage("dirLight.shadowMap", 3);
        SoglProgramManager::bindImage("ssaoMap", 4);
        SoglProgramManager::bindImage("skyIrradiance", 5);
        SoglProgramManager::bindImage("prefilterMap", 6);
        SoglProgramManager::bindImage("brdfLUT", 7);

        updateLightingShaderInputs();
    }
#pragma endregion updateLightingProgram

#pragma region rendererToggles
    void SoglRenderer::toggleShadows(const bool state){
        if (shadowEnabled == state) return;

        shadowEnabled = state;
        updateLighting();
    }

    void SoglRenderer::toggleSSAO(const bool state){
        if (ssaoEnabled == state) return;

        ssaoEnabled = state;
        updateLighting();
    }

    void SoglRenderer::toggleSSAOBlur(const bool state){
        ssaoBlurEnabled = state;
    }

    void SoglRenderer::toggleIrradiance(const bool state){
        if (irradianceEnabled == state) return;

        irradianceEnabled = state;
        updateLighting();
    }
#pragma endregion rendererToggles
} // namespace sogl
