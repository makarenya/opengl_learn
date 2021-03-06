#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"

class TSilhouetteShader: public TShaderProgram {
private:
    GLint Model;
public:
    explicit TSilhouetteShader(const TUniformBindingBase &matrices)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_silhouette_vert)
            .SetFragment(&NResource::shaders_silhouette_frag)
            .SetBlock("Matrices", matrices))
          , Model(DefineProp("model")) {
    }

    friend class TSilhouetteSetup;
};

class TSilhouetteSetup: public TShaderSetup {
private:
    const TSilhouetteShader *Shader;

public:
    explicit TSilhouetteSetup(const TSilhouetteShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TSilhouetteSetup(TSilhouetteSetup &&src) noexcept
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TSilhouetteSetup() override {
        if (Shader != nullptr) {
            try {
                Set(Shader->Model, glm::mat4(0));
            } catch (...) {
            }
        }
    }

    TSilhouetteSetup &&SetModel(glm::mat4 model) {
        Set(Shader->Model, model);
        return std::move(*this);
    }
};
