#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

//std
#include <map>
#include <string>

namespace sogl
{
    
    class SoglProgramManager{
        public:
        static GLuint addProgram(std::string vertProgramName, std::string fragProgramName, std::string shaderParams);
        static GLuint addProgram(std::string programName, std::string shaderParams);
        static GLuint addProgram(std::string programName);

        static GLuint recompileProgram(std::string vertProgramName, std::string fragProgramName, std::string shaderParams);
        static GLuint recompileProgram(std::string programName, std::string shaderParams);
        static void useProgram(std::string programName);

        static void setVec3(const std::string uniformName, glm::vec3 value);
        static void setMat4(const std::string uniformName, glm::mat4 value);
        static void setFloat(const std::string uniformName, float value);
        static void setInt(const std::string uniformName, int value);
        static void bindImage(const std::string uniformName, GLuint val);

        static std::string currentProgram;
        static GLuint currentProgramId;

        static std::string programPath;
        static std::map <std::string, GLuint> activePrograms;

        static std::string defaultShader;
        static std::string shadowSuffix;
    };
} // namespace sogl
