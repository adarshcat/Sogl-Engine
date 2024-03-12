
#include "sogl_shader_loader.hpp"
#include "sogl_program_manager.hpp"

#include <cstring>
#include <iostream>

namespace sogl
{
    std::string SoglProgramManager::programPath = "assets/shaders/";
    std::string SoglProgramManager::currentProgram = "";
    std::map <std::string, GLuint> SoglProgramManager::activePrograms;
    GLuint SoglProgramManager::currentProgramId = 0;

    GLuint SoglProgramManager::addProgram(std::string programName){
        for (auto &[key, value]: activePrograms){
            if (std::strcmp(key.c_str(), programName.c_str()) == 0){
                return value;
            }
        }

        std::string vertexPath = programPath + programName + ".vert";
        std::string fragmentPath = programPath + programName + ".frag";

        GLuint programID = LoadShaders(vertexPath.c_str(), fragmentPath.c_str());
        activePrograms.insert({programName, programID});

        return programID;
    }
    
    void SoglProgramManager::useProgram(std::string programName){
        if (currentProgram == programName) return;

        //std::cout << "Using a different program" << std::endl;
        glUseProgram(activePrograms[programName]);
        currentProgram = programName;
        currentProgramId = activePrograms[programName];
    }

    void SoglProgramManager::setVec3(const std::string uniformName, glm::vec3 value){
        glUniform3f(glGetUniformLocation(currentProgramId, uniformName.c_str()), value.x, value.y, value.z);
    }

    void SoglProgramManager::setMat4(const std::string uniformName, glm::mat4 value){
        glUniformMatrix4fv(glGetUniformLocation(currentProgramId, uniformName.c_str()),  1, GL_FALSE, &value[0][0]);
    }

    void SoglProgramManager::bindImage(const std::string uniformName, GLuint val){
        glUniform1i(glGetUniformLocation(currentProgramId, uniformName.c_str()), val);
    }
} // namespace sogl
