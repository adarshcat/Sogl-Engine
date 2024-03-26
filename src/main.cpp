#include "engine/sogl_engine.hpp"
#include "engine/sogl_game_object.hpp"
#include "util/sogl_model_loader.hpp"

// std
#include <iostream>

using namespace sogl;

int main() {
    SoglEngine engine;

    std::vector<std::unique_ptr<SoglGameObject>> testSceneModels = SoglModelLoader::loadModel("testScene.glb");
    std::vector<std::unique_ptr<SoglGameObject>> monkeyModel = SoglModelLoader::loadModel("monkey.obj");

    engine.addGameObjects(std::move(testSceneModels));
    engine.addGameObjects(std::move(monkeyModel));
    
    engine.run();

    return 0;
}