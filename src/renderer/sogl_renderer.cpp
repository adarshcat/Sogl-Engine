
#include "sogl_renderer.hpp"

// std
#include <iostream>

namespace sogl
{
    // Setups renderin onto a glfw window
    SoglRenderer::SoglRenderer(SoglWindow& wind, const int width, const int height):
        WIDTH{width}, HEIGHT{height}, soglWindow{wind}
    {
        glewExperimental =  GL_TRUE;

        if (soglWindow.initialiseWindow() == false){
            std::cout << "Failed to initialise the window" << std::endl;
            exit(EXIT_FAILURE);
        } else std::cout << "Window initialised" << std::endl;

        if (glewInit() != GLEW_OK) {
            std::cout << "Failed to initialise GLEW" << std::endl;
            exit(EXIT_FAILURE);
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        std::cout << "App initialised" << std::endl;
    }


    // Renders onto glfw window, takes in all the renderable game objects and calls draw() on them
    bool SoglRenderer::draw(std::vector<SoglGameObject> &gameObjects, glm::mat4 viewProjectionMatrix, glm::vec3 camPos){
        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // // Draw calls to all the game objects
        for (SoglGameObject &gameObj : gameObjects){
            gameObj.draw(viewProjectionMatrix, camPos);
        }

        return !soglWindow.updateAndPollWindow();
    }
} // namespace sogl
