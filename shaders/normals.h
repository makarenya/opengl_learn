#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TNormalsShader: public TShaderProgram {
public:
    TNormalsShader()
        : TShaderProgram("shaders/normals.vert", "shaders/normals.frag", "shaders/normals.geom") {
    }
};

class TNormalsSetup: public TProgramSetup {
public:
    TNormalsSetup(const TNormalsShader &shader)
        : TProgramSetup(shader) {
    }

    TNormalsSetup &&Model(glm::mat4 model) {
        Set("model", model);
        return std::move(*this);
    }
};
