#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"

class TNormalsShader: public TShaderProgram {
private:
    GLint Model;
public:
    explicit TNormalsShader(const TUniformBindingBase &matrices)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_normals_vert)
            .SetFragment(&NResource::shaders_normals_frag)
            .SetGeometry(&NResource::shaders_normals_geom)
            .SetBlock("Matrices", matrices))
          , Model(DefineProp("model")) {
    }

    friend class TNormalsSetup;
};

class TNormalsSetup: public TShaderSetup {
private:
    const TNormalsShader *Shader;

public:
    explicit TNormalsSetup(const TNormalsShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TNormalsSetup(TNormalsSetup &&src) noexcept
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TNormalsSetup() override {
        if (Shader != nullptr) {
            try {
                Set(Shader->Model, glm::mat4(0));
            } catch (...) {
            }
        }
    }

    TNormalsSetup &&SetModel(glm::mat4 model) {
        Set(Shader->Model, model);
        return std::move(*this);
    }
};
