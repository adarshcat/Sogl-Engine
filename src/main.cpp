#include "engine/sogl_engine.hpp"
#include "engine/sogl_game_object.hpp"
#include "util/sogl_model_loader.hpp"

// std
#include <iostream>

using namespace sogl;

int main() {
    SoglEngine engine;
    std::vector<std::unique_ptr<SoglGameObject>> monkeyModel = SoglModelLoader::loadModel("monkey.obj");
    std::vector<std::unique_ptr<SoglGameObject>> planeModel = SoglModelLoader::loadModel("plane.glb");
    planeModel[0]->material.albedo = glm::vec3(1.0, 0.0, 0.0);

    planeModel[0]->translate(glm::vec3(0, -2.1, 0));

    engine.addGameObjects(std::move(monkeyModel));
    engine.addGameObjects(std::move(planeModel));

    engine.run();

    return 0;
}