#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TSkyboxShader: public TShaderProgram {
private:
    int SkyBox;
public:
    explicit TSkyboxShader(const TUniformBindingBase &matrices)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex("shaders/skybox.vert")
            .SetFragment("shaders/skybox.frag")
            .SetBlock("Matrices", matrices))
          , SkyBox(DefineTexture("skybox")) {
    }

    friend class TSkyBoxSetup;
};

class TSkyBoxSetup: public TShaderSetup {
private:
    const TSkyboxShader *Shader;

public:
    explicit TSkyBoxSetup(const TSkyboxShader *shader)
        : TShaderSetup(shader)
          , Shader(shader) {
    }

    TSkyBoxSetup &&SetSkyBox(const TTexture &texture) {
        Set(Shader->SkyBox, texture);
        return std::move(*this);
    }
};
