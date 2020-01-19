#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TParticlesShader: public TShaderProgram {
private:
    GLint Model;
    GLint Single;
    GLint ViewPos;
    int SkyBox;

public:
    explicit TParticlesShader(const TUniformBindingBase &matrices,
                              const TUniformBindingBase &lights,
                              const TUniformBindingBase &lightsPos)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&shaders_particles_vert, shaders_particles_vert_len)
            .SetFragment(&shaders_particles_frag, shaders_particles_frag_len)
            .SetBlock("Matrices", matrices)
            .SetBlock("Lights", lights)
            .SetBlock("LightsPos", lightsPos))
          , SkyBox(DefineTexture("skybox"))
          , Model(DefineProp("model"))
          , Single(DefineProp("single"))
          , ViewPos(DefineProp("viewPos")) {
    }

    friend class TParticlesSetup;
};

class TParticlesSetup: public TShaderSetup {
private:
    const TParticlesShader *Shader;

public:
    explicit TParticlesSetup(const TParticlesShader *shader)
        : TShaderSetup(shader)
          , Shader(shader) {
    }

    TParticlesSetup(TParticlesSetup &&src) noexcept
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TParticlesSetup() override {
        if (Shader != nullptr) {
            try {
                Set(Shader->Model, glm::mat4(0));
                Set(Shader->Single, glm::mat4(0));
                Set(Shader->ViewPos, glm::vec3(0));
            } catch (...) {
            }
        }
    }

    TParticlesSetup &&SetModel(glm::mat4 model) {
        Set(Shader->Model, model);
        return std::move(*this);
    }

    TParticlesSetup &&SetSingle(glm::mat4 model) {
        Set(Shader->Single, model);
        return std::move(*this);
    }

    TParticlesSetup &&SetViewPos(glm::vec3 position) {
        Set(Shader->ViewPos, position);
        return std::move(*this);
    }

    TParticlesSetup &&SetSkyBox(TCubeTexture &texture) {
        Set(Shader->SkyBox, texture);
        return std::move(*this);
    }
};
