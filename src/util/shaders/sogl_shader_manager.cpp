
#include "sogl_shader_manager.hpp"


namespace sogl
{
    
    SoglShaderManager::SoglShaderManager(){

    }

    void SoglShaderManager::addShader(SoglShader shader){
        shaders.push_back(shader);
    }

    // Returns the material reference at specified index in array
    SoglShader& SoglShaderManager::getShader(const int i){
        return shaders[i];
    }
} // namespace sogl
