#pragma once 

#include <glm/gtc/matrix_transform.hpp>
#include "sogl_shader.hpp"

//std
#include <vector>

namespace sogl
{
    
    class SoglShaderManager{
        public:
        SoglShaderManager();

        void addShader(SoglShader shader);
        SoglShader& getShader(const int i);

        private:
        std::vector <SoglShader> shaders;
    };
} // namespace sogl
