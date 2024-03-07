
#include "sogl_shader.hpp"

namespace sogl
{
    
    SoglShader::SoglShader(SoglProgramManager* progMang){
        programManager = progMang;
        programId = programManager->addProgram("default");
        this->programName = "default";
    }

    SoglShader::SoglShader(SoglProgramManager* progMang, std::string programName){
        programManager = progMang;
        programId = programManager->addProgram(programName);
        this->programName = programName;
    }

    void SoglShader::applyShader(){
        programManager->useProgram(programName);
    }

    void SoglShader::setVec3(const std::string uniformName, glm::vec3 value){
        glUniform3f(glGetUniformLocation(programId, uniformName.c_str()), value.x, value.y, value.z);
    }

    void SoglShader::setMat4(const std::string uniformName, glm::mat4 value){
        glUniformMatrix4fv(glGetUniformLocation(programId, uniformName.c_str()),  1, GL_FALSE, &value[0][0]);
    }
} // namespace sogl
