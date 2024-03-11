
#include "sogl_shader_loader.hpp"
#include "sogl_program_manager.hpp"

#include <cstring>
#include <iostream>

namespace sogl
{
    SoglProgramManager::SoglProgramManager(){

    }

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

        std::cout << "Using a different program" << std::endl;
        glUseProgram(activePrograms[programName]);
        currentProgram = programName;
    }
} // namespace sogl
