#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/env/sogl_camera.hpp"

namespace sogl
{
    
    struct SoglLightingData{
        CameraData camData;
        glm::mat4 dirLightMatrix;

        GLuint gBuffer;
        GLuint gDepth;
        GLuint gNormalMet;
        GLuint gAlbedoSpec;

        GLuint shadowMap;

        GLuint ssaoBlurOutput;
        GLuint ssaoOutput;

        GLuint skyboxDiffuseIrradiance;
        GLuint skyboxPrefilterMap;
        GLuint skyboxBrdfLUT;
    };

    struct SoglLightingSettings{
        bool shadowEnabled = true;
        bool ssaoEnabled = true;
        bool ssaoBlurEnabled = true;
        bool irradianceEnabled = true;
    };

} // namespace sogl
