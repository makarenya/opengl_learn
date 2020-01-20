#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"

class TLightShader: public TShaderProgram {
private:
    GLint Model;
    GLint Color;
public:
    explicit TLightShader(const TUniformBindingBase &matrices)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_light_vert)
            .SetFragment(&NResource::shaders_light_frag))
          , Model(DefineProp("model"))
          , Color(DefineProp("lightColor")) {
    }

    friend class TLightSetup;
};

class TLightSetup: public TShaderSetup {
private:
    const TLightShader *Shader;

public:
    explicit TLightSetup(const TLightShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TLightSetup(TLightSetup &&src) noexcept
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TLightSetup() override {
        if (Shader != nullptr) {
            try {
                Set(Shader->Color, glm::vec4(0));
                Set(Shader->Model, glm::mat4(0));
            } catch (...) {
            }
        }
    }

    TLightSetup &SetColor(glm::vec3 color) {
        Set(Shader->Color, color);
        return *this;
    }

    TLightSetup &SetModel(glm::mat4 model) {
        Set(Shader->Model, model);
        return *this;
    }
};
