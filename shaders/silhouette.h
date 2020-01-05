#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TSilhouetteSetup {
public:
    TProgramSetup Setup;

    TSilhouetteSetup(TProgramSetup &&setup)
        : Setup(std::move(setup)) {
    }

    TSilhouetteSetup &&Model(glm::mat4 model) {
        Setup.Set("model", model);
        return std::move(*this);
    }
};

class TSilhouetteShader {
    TShaderProgram Program;
public:
    TSilhouetteShader()
        : Program("shaders/silhouette.vert", "shaders/silhouette.frag") {
    }

    TSilhouetteSetup Use(glm::mat4 projection, glm::mat4 view) {
        return {Program.Use().Set("projection", projection).Set("view", view)};
    }
};
