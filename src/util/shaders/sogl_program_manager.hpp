#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

//std
#include <map>

namespace sogl
{
    
    class SoglProgramManager{
        public:
        static GLuint addProgram(std::string programName);
        static void useProgram(std::string programName);

        static void setVec3(const std::string uniformName, glm::vec3 value);
        static void setMat4(const std::string uniformName, glm::mat4 value);
        static void bindImage(const std::string uniformName, GLuint val);

        static std::string currentProgram;
        static GLuint currentProgramId;

        static std::string programPath;
        static std::map <std::string, GLuint> activePrograms;
    };
} // namespace sogl
