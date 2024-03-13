
#include "engine/sogl_engine.hpp"
#include "engine/sogl_game_object.hpp"
#include "util/sogl_model_loader.hpp"

// std
#include <iostream>

using namespace sogl;

int main() {
    SoglEngine engine;
    std::vector<SoglGameObject> monkeyModel = SoglModelLoader::loadModel("monkey.obj");
    std::vector<SoglGameObject> planeModel = SoglModelLoader::loadModel("plane.obj");

    planeModel[0].translate(glm::vec3(0, -2.2, 0));

    engine.addGameObjects(monkeyModel);
    engine.addGameObjects(planeModel);

    engine.run();

    return 0;
}