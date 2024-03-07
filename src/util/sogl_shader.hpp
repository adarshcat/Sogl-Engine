#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "sogl_program_manager.hpp"

//std
#include <string>

namespace sogl
{
    
    class SoglShader{
        public:
        SoglShader(SoglProgramManager* progMang);
        SoglShader(SoglProgramManager* progMang, std::string programName);
        glm::vec3 albedo = glm::vec3(0.0);

        void setVec3(const std::string uniformName, glm::vec3 value);
        void setMat4(const std::string uniformName, glm::mat4 value);
        
        void applyShader();

        private:
        SoglProgramManager* programManager;
        std::string programName;

        GLuint programId;
    };
} // namespace sogl
