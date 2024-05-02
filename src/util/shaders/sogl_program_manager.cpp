
#include "util/shaders/sogl_program_manager.hpp"
#include "util/shaders/sogl_shader_loader.hpp"

#include <cstring>
#include <iostream>

namespace sogl
{
    std::string SoglProgramManager::programPath = "assets/shaders/";
    std::string SoglProgramManager::currentProgram = "";
    std::map <std::string, GLuint> SoglProgramManager::activePrograms;
    GLuint SoglProgramManager::currentProgramId = 0;

    std::string SoglProgramManager::defaultShader = "default/default";
    std::string SoglProgramManager::shadowSuffix = "_shadow";

    GLuint SoglProgramManager::addProgram(std::string vertProgramName, std::string fragProgramName, std::string shaderParams){
        std::string programName = fragProgramName;

        for (auto &[key, value]: activePrograms){
            if (std::strcmp(key.c_str(), programName.c_str()) == 0){
                return value;
            }
        }

        std::string vertexPath = programPath + vertProgramName + ".vert";
        std::string fragmentPath = programPath + fragProgramName + ".frag";

        GLuint programID = LoadShaders(vertexPath.c_str(), fragmentPath.c_str(), shaderParams);

        activePrograms.insert({programName, programID});

        currentProgram = "";
        currentProgramId = -1;

        return programID;
    }

    GLuint SoglProgramManager::addProgram(std::string programName, std::string shaderParams){
        return addProgram(programName, programName, shaderParams);
    }

    GLuint SoglProgramManager::addProgram(std::string programName){
        return addProgram(programName, "");
    }


    GLuint SoglProgramManager::recompileProgram(std::string vertProgramName, std::string fragProgramName, std::string shaderParams){
        std::string programName = fragProgramName;
        // Check if the program is present
        bool present = false;
        for (auto &[key, value]: activePrograms){
            if (std::strcmp(key.c_str(), programName.c_str()) == 0){
                present = true;
            }
        }

        // If not present, just add it
        if (!present)
            return addProgram(vertProgramName, fragProgramName, shaderParams);

        // free old program id from gpu, erase reference from activePrograms map
        glDeleteProgram(activePrograms[programName]);
        activePrograms.erase(programName);

        return addProgram(vertProgramName, fragProgramName, shaderParams);
    }

    GLuint SoglProgramManager::recompileProgram(std::string programName, std::string shaderParams){
        return recompileProgram(programName, programName, shaderParams);
    }
    
    
    void SoglProgramManager::useProgram(std::string programName){
        if (currentProgram == programName) return;

        if (activePrograms.find(programName) == activePrograms.end()){
            std::cout << "Sogl Program manager: shader program not present!" << std::endl;
            return;
        }

        glUseProgram(activePrograms[programName]);
        currentProgram = programName;
        currentProgramId = activePrograms[programName];
    }

    // helper functions to change active program's uniforms
    void SoglProgramManager::setVec3(const std::string uniformName, glm::vec3 value){
        glUniform3f(glGetUniformLocation(currentProgramId, uniformName.c_str()), value.x, value.y, value.z);
    }

    void SoglProgramManager::setVec2(const std::string uniformName, glm::vec2 value){
        glUniform2f(glGetUniformLocation(currentProgramId, uniformName.c_str()), value.x, value.y);
    }

    void SoglProgramManager::setMat4(const std::string uniformName, glm::mat4 value){
        glUniformMatrix4fv(glGetUniformLocation(currentProgramId, uniformName.c_str()),  1, GL_FALSE, &value[0][0]);
    }

    void SoglProgramManager::setFloat(const std::string uniformName, float value){
        glUniform1f(glGetUniformLocation(currentProgramId, uniformName.c_str()), value);
    }

    void SoglProgramManager::setInt(const std::string uniformName, int value){
        glUniform1i(glGetUniformLocation(currentProgramId, uniformName.c_str()), value);
    }
    
    void SoglProgramManager::bindImage(const std::string uniformName, GLuint val){
        glUniform1i(glGetUniformLocation(currentProgramId, uniformName.c_str()), val);
    }
} // namespace sogl
