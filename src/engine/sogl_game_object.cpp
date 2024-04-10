
#include "engine/sogl_game_object.hpp"

//std
#include <iostream>

namespace sogl
{
    
    SoglGameObject::SoglGameObject(glm::mat4 _transform) {
        modelMatrix = _transform;
    }

#pragma region transformationFunctions
    void SoglGameObject::translate(glm::vec3 _amnt){
        modelMatrix = glm::translate(modelMatrix, _amnt);
    }

    void SoglGameObject::rotate(glm::vec3 _axis, float _angle){
        modelMatrix = glm::rotate(modelMatrix, _angle, _axis);
    }

    void SoglGameObject::scale(glm::vec3 _scaleVec){
        modelMatrix = glm::scale(modelMatrix, _scaleVec);
    }

    void SoglGameObject::setTransform(glm::mat4 _transform){
        modelMatrix = _transform;
    }
#pragma endregion transformationFunctions
} // namespace sogl

