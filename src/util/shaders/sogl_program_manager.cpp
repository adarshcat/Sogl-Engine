
#include "util/shaders/sogl_program_manager.hpp"
#include "util/shaders/sogl_shader_loader.hpp"

#include <cstring>
#include <iostream>

namespace sogl
{
    std::string SoglProgramManager::programPath = "assets/shaders/";
    std::string SoglProgramManager::currentProgram = "";
    std::unordered_map <std::string, GLuint> SoglProgramManager::activePrograms;
    std::unordered_map <std::string, std::unordered_map <std::string, GLuint>> SoglProgramManager::uniformMap;
    GLuint SoglProgramManager::currentProgramId = 0;

    std::string SoglProgramManager::defaultShader = "default/default";
    std::string SoglProgramManager::defaultShaderTransparent = "default/default_transparent";
    std::string SoglProgramManager::shadowSuffix = "_shadow";

#pragma region addPrograms
    GLuint SoglProgramManager::addProgram(std::string vertProgramName, std::string fragProgramName, std::string shaderParams){
        std::string programName = fragProgramName;

        // check if the program is already in the map, if yes, return it's id
        for (auto &[key, value]: activePrograms){
            if (std::strcmp(key.c_str(), programName.c_str()) == 0){
                return value;
            }
        }

        // if not already in the map, load the shader program and return it's id
        std::string vertexPath = programPath + vertProgramName + ".vert";
        std::string fragmentPath = programPath + fragProgramName + ".frag";

        GLuint programID = LoadShaders(vertexPath.c_str(), fragmentPath.c_str(), shaderParams);

        activePrograms.insert({programName, programID});
        uniformMap.insert({programName, std::unordered_map <std::string, GLuint>{}});

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
        uniformMap.erase(programName);

        return addProgram(vertProgramName, fragProgramName, shaderParams);
    }

    GLuint SoglProgramManager::recompileProgram(std::string programName, std::string shaderParams){
        return recompileProgram(programName, programName, shaderParams);
    }
    
#pragma endregion addPrograms


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


#pragma region uniformChangeFunc
    // gets uniform location, fetches/adds to cache
    GLuint SoglProgramManager::getUniformLocation(std::string uniformName){
        std::unordered_map <std::string, GLuint> &subUniformMap = uniformMap[currentProgram];

        // if uniform not already in cache, add it to cache
        if (subUniformMap.find(uniformName) == subUniformMap.end()){
            //std::cout << "Adding to cache " << currentProgram << " " << uniformName << '\n';
            subUniformMap[uniformName] = glGetUniformLocation(currentProgramId, uniformName.c_str());
        }

        return subUniformMap[uniformName];
    }

    // helper functions to change active program's uniforms
    void SoglProgramManager::setVec3(const std::string uniformName, glm::vec3 value){
        glUniform3f(getUniformLocation(uniformName), value.x, value.y, value.z);
    }

    void SoglProgramManager::setVec2(const std::string uniformName, glm::vec2 value){
        glUniform2f(getUniformLocation(uniformName), value.x, value.y);
    }

    void SoglProgramManager::setMat4(const std::string uniformName, glm::mat4 value){
        glUniformMatrix4fv(getUniformLocation(uniformName),  1, GL_FALSE, &value[0][0]);
    }

    void SoglProgramManager::setFloat(const std::string uniformName, float value){
        glUniform1f(getUniformLocation(uniformName), value);
    }

    void SoglProgramManager::setInt(const std::string uniformName, int value){
        glUniform1i(getUniformLocation(uniformName), value);
    }
    
    void SoglProgramManager::bindImage(const std::string uniformName, GLuint val){
        glUniform1i(getUniformLocation(uniformName), val);
    }
#pragma endregion uniformChangeFunc

} // namespace sogl
