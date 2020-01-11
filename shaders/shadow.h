#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TShadowShader: public TShaderProgram {
private:
    GLint LightMatrix;
    GLint Model;
    GLint Opacity;
public:
    explicit TShadowShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex("shaders/shadow.vert")
            .SetFragment("shaders/shadow.frag")
            .SetTexture("diffuse", EMaterialProp::Diffuse))
          , LightMatrix(DefineProp("lightMatrix"))
          , Model(DefineProp("model"))
          , Opacity(DefineProp("opacity")) {
    }

    friend class TShadowSetup;
};

class TShadowSetup: public TShaderSetup {
private:
    const TShadowShader *Shader;

public:
    explicit TShadowSetup(const TShadowShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TShadowSetup(TShadowSetup &&src) noexcept
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TShadowSetup() override {
        if (Shader != nullptr) {
            try {
                Set(Shader->LightMatrix, glm::mat4(0));
                Set(Shader->Model, glm::mat4(0));
            } catch (...) {
            }
        }
    }

    TShadowSetup &&SetLightMatrix(glm::mat4 model) {
        Set(Shader->LightMatrix, model);
        return std::move(*this);
    }

    TShadowSetup &&SetModel(glm::mat4 model) {
        Set(Shader->Model, model);
        return std::move(*this);
    }

    TShadowSetup &&SetOpacity(bool opacity) {
        Set(Shader->Opacity, opacity);
        return std::move(*this);
    }
};
