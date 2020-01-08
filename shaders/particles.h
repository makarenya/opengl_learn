#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TParticlesShader: public TShaderProgram {
private:
    GLint Model;
    GLint ViewPos;
    int SkyBox;

public:
    explicit TParticlesShader(const TUniformBindingBase &matrices, const TUniformBindingBase &lights)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex("shaders/particles.vert")
            .SetFragment("shaders/particles.frag")
            .SetBlock("Matrices", matrices)
            .SetBlock("Lights", lights))
          , SkyBox(DefineTexture("skybox"))
          , Model(DefineProp("model"))
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
                Set(Shader->ViewPos, glm::vec3(0));
            } catch (...) {
            }
        }
    }

    TParticlesSetup &&SetModel(glm::mat4 model) {
        Set(Shader->Model, model);
        return std::move(*this);
    }

    TParticlesSetup &&SetViewPos(glm::vec3 position) {
        Set(Shader->ViewPos, position);
        return std::move(*this);
    }

    TParticlesSetup &&SetSkyBox(TTexture &texture) {
        Set(Shader->SkyBox, texture);
        return std::move(*this);
    }
};
