#pragma once

#include <GL/glew.h>

//std
#include <string>
#include <map>

namespace sogl
{
    
    class SoglProgramManager{
        public:
        SoglProgramManager();
        GLuint addProgram(std::string programName);
        void useProgram(std::string programName);

        private:
        std::string currentProgram = "";

        std::string programPath = "assets/shaders/";
        std::map <std::string, GLuint> activePrograms;
    };
} // namespace sogl
