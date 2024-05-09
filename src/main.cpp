
#include "stb_image.h"

#include "engine/sogl_engine.hpp"
#include "engine/game/sogl_game_object.hpp"
#include "util/sogl_model_loader.hpp"

// std
#include <iostream>

using namespace sogl;

int main() {
    SoglEngine engine;

    // std::vector<std::unique_ptr<SoglGameObject>> cBoxModels = SoglModelLoader::loadModel("cBox.glb");
    // std::vector<std::unique_ptr<SoglGameObject>> monkeyModel = SoglModelLoader::loadModel("monkey.obj");

    // float scFactor = 1.0;
    // for (int i=0; i<cBoxModels.size(); i++)
    //     cBoxModels[i]->scale(glm::vec3(5.0 * scFactor));
    
    // monkeyModel[0]->translate(glm::vec3(10, 2, 0));
    // monkeyModel[0]->scale(glm::vec3(1.5 * scFactor));

    // engine.addGameObjects(std::move(monkeyModel));
    // engine.addGameObjects(std::move(cBoxModels));

    std::vector<std::unique_ptr<SoglGameObject>> carModel = SoglModelLoader::loadModel("car.glb");
    std::vector<std::unique_ptr<SoglGameObject>> planeModel = SoglModelLoader::loadModel("plane.glb");

    for (int i=0; i<carModel.size(); i++){
        carModel[i]->scale(glm::vec3(50.0));
        carModel[i]->rotate(glm::vec3(1, 0, 0), -3.14/2.0);
    }

    engine.addGameObjects(std::move(carModel));
    engine.addGameObjects(std::move(planeModel));

    engine.run();

    return 0;
}