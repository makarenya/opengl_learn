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
    TSilhouetteSetup(const TSilhouetteShader &shader)
        : TProgramSetup(shader) {
    }

    TSilhouetteSetup &&Model(glm::mat4 model) {
        Set("model", model);
        return std::move(*this);
    }
};
