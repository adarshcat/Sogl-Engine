
#include "renderer/sogl_window.hpp"

namespace sogl
{
    
    SoglWindow::SoglWindow(const int w, const int h, const char *windowName): WIDTH{w}, HEIGHT{h}, windowName{windowName} {
        
    }

    SoglWindow::~SoglWindow(){
        glfwTerminate();
    }

    bool SoglWindow::initialiseWindow(){
        if (!glfwInit()){
            return false;
        }

        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow( WIDTH, HEIGHT, windowName, NULL, NULL);

        if(window == NULL){
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(window);
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);
        //glfwSwapInterval(1);

        return true;
    }

    
    // Polls the window for events and returns whether it should close. Called every frame drawn
    bool SoglWindow::updateAndPollWindow(){
        glfwSwapBuffers(window);
        glfwPollEvents();

        return (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
    }
} // namespace sogl
