#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TParticlesShader: public TShaderProgram {
public:
    TParticlesShader()
        : TShaderProgram("shaders/particles.vert", "shaders/particles.frag") {
    }
};

class TParticlesSetup: public TProgramSetup {
public:
    TParticlesSetup(const TParticlesShader &shader)
        : TProgramSetup(shader) {
        Texture("skybox", GL_TEXTURE_CUBE_MAP);
    }

    TParticlesSetup &&ViewPos(glm::vec3 viewPos) {
        Set("viewPos", viewPos);
        return std::move(*this);
    }

    TParticlesSetup &&Model(glm::mat4 model) {
        Set("model", model);
        return std::move(*this);
    }

    TParticlesSetup &&Skybox(const TCubeTexture &texture) {
        texture.Bind(TextureLoc("skybox"));
        return std::move(*this);
    }
};
