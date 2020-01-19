#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../shader_program.h"

class TShadowShader: public TShaderProgram {
private:
    GLint LightMatrices;
    GLint Direct;
    GLint Model;
    GLint LightPos;
    GLint Opacity;
public:
    explicit TShadowShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&shaders_shadow_vert, shaders_shadow_vert_len)
            .SetFragment(&shaders_shadow_frag, shaders_shadow_frag_len)
            .SetGeometry(&shaders_shadow_geom, shaders_shadow_geom_len)
            .SetTexture(EMaterialProp::Diffuse, "diffuse"))
          , LightMatrices(DefineProp("lightMatrices"))
          , Direct(DefineProp("direct"))
          , Model(DefineProp("model"))
          , LightPos(DefineProp("lightPos"))
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
                Set(Shader->Direct, false);
                Set(Shader->Model, glm::mat4(0));
            } catch (...) {
            }
        }
    }

    TShadowSetup &&SetDirect(bool direct) {
        Set(Shader->Direct, direct);
        return std::move(*this);
    }

    TShadowSetup &&SetLightMatrices(std::array<glm::mat4, 6> mat) {
        Set(Shader->LightMatrices, mat.data(), 6);
        return std::move(*this);
    }

    TShadowSetup &&SetModel(glm::mat4 model) {
        Set(Shader->Model, model);
        return std::move(*this);
    }

    TShadowSetup &&SetLightPos(glm::vec3 lightPos) {
        Set(Shader->LightPos, lightPos);
        return std::move(*this);
    }

    TShadowSetup &&SetOpacity(bool opacity) {
        Set(Shader->Opacity, opacity);
        return std::move(*this);
    }
};
