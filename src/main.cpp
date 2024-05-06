
#include "stb_image.h"

#include "engine/sogl_engine.hpp"
#include "engine/game/sogl_game_object.hpp"
#include "util/sogl_model_loader.hpp"

// std
#include <iostream>

using namespace sogl;

int main() {
    SoglEngine engine;

    std::vector<std::unique_ptr<SoglGameObject>> cBoxModels = SoglModelLoader::loadModel("cBox.glb");
    std::vector<std::unique_ptr<SoglGameObject>> monkeyModel = SoglModelLoader::loadModel("monkey.obj");
    std::vector<std::unique_ptr<SoglGameObject>> planeModel = SoglModelLoader::loadModel("plane.obj");

    float scFactor = 1.0;
    for (int i=0; i<cBoxModels.size(); i++)
        cBoxModels[i]->scale(glm::vec3(5.0 * scFactor));
    
    monkeyModel[0]->translate(glm::vec3(10, 2, 0));
    monkeyModel[0]->scale(glm::vec3(1.5 * scFactor));

    planeModel[0]->translate(glm::vec3(0, -4, 0));
    static_cast<SoglMeshObject*>(planeModel[0].get())->material.transparent = true;
    static_cast<SoglMeshObject*>(planeModel[0].get())->material.alpha = 0.5;
    static_cast<SoglMeshObject*>(planeModel[0].get())->material.albedo = glm::vec3(0.0, 0.5, 0.8);

    engine.addGameObjects(std::move(monkeyModel));
    engine.addGameObjects(std::move(cBoxModels));
    engine.addGameObjects(std::move(planeModel));
    

    engine.run();

    return 0;
}