#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace sogl {
    
    class SoglWindow{
        public:
        SoglWindow(const int w, const int h, const char *windowName);
        ~SoglWindow();

        SoglWindow(const SoglWindow&) = delete;
        SoglWindow operator=(const SoglWindow&) = delete;
        
        GLFWwindow* window;
        const int WIDTH;
        const int HEIGHT;
        const char *windowName;

        bool initialiseWindow();
        bool updateAndPollWindow();
    };
} // namespace sogl
