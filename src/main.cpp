#include "engine/sogl_engine.hpp"
#include "engine/sogl_game_object.hpp"
#include "util/sogl_model_loader.hpp"

// std
#include <iostream>

using namespace sogl;

int main() {
    SoglEngine engine;

    std::vector<std::unique_ptr<SoglGameObject>> testSceneModels = SoglModelLoader::loadModel("cBox.glb");

    for (int i=0; i<testSceneModels.size(); i++)
        testSceneModels[i]->scale(glm::vec3(6.0));

    engine.addGameObjects(std::move(testSceneModels));
    
    engine.run();

    return 0;
}