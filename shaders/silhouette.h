#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TSilhouetteShader: public TShaderProgram {
public:
    TSilhouetteShader()
        : TShaderProgram("shaders/silhouette.vert", "shaders/silhouette.frag") {
    }
};

class TSilhouetteSetup: public TProgramSetup {
public:
    TSilhouetteSetup(const TSilhouetteShader &shader, glm::mat4 projection, glm::mat4 view)
        : TProgramSetup(shader) {
        Set("projection", projection);
        Set("view", view);
    }

    TSilhouetteSetup &&Model(glm::mat4 model) {
        Set("model", model);
        return std::move(*this);
    }
};
