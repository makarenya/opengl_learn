#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TParticlesShader: public TShaderProgram {
public:
    TParticlesShader()
        : TShaderProgram("shaders/particles.vert", "shaders/particles.frag", "shaders/particles.geom") {
    }
};

class TParticlesSetup: public TProgramSetup {
public:
    TParticlesSetup(const TParticlesShader &shader)
        : TProgramSetup(shader) {
    }

    TParticlesSetup &&Model(glm::mat4 model) {
        Set("model", model);
        return std::move(*this);
    }
};
