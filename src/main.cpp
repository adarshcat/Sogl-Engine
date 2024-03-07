
#include "sogl_engine.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// std
#include <iostream>

int main() {
    sogl::SoglEngine engine;
    engine.run();

    return 0;
}