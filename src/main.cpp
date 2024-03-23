#include "engine/sogl_engine.hpp"
#include "engine/sogl_game_object.hpp"
#include "util/sogl_model_loader.hpp"

// std
#include <iostream>

using namespace sogl;

int main() {
    SoglEngine engine;

    std::vector<std::unique_ptr<SoglGameObject>> testSceneModels = SoglModelLoader::loadModel("testScene.glb");
    testSceneModels[0]->material.albedo = glm::vec3(1.0, 0.0, 0.0);
    engine.addGameObjects(std::move(testSceneModels));
    
    engine.run();

    return 0;
}