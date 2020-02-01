#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"

class THdrShader: public TShaderProgram {
    GLint Screen;
    GLint Depth;
    GLint Bloom;
    GLint Exposure;

public:
    THdrShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_border_vert)
            .SetFragment(&NResource::shaders_hdr_frag))
          , Screen(DefineTexture("screenTexture"))
          , Depth(DefineTexture("depthTexture"))
          , Bloom(DefineTexture("bloomTexture"))
          , Exposure(DefineProp("exposure")) {
    }
    friend class THdrSetup;
};

class THdrSetup: public TShaderSetup {
private:
    const THdrShader *Shader;

public:
    explicit THdrSetup(const THdrShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    THdrSetup &&SetScreen(const TFlatTexture &texture) {
        Set(Shader->Screen, texture);
        return std::move(*this);
    }

    THdrSetup &&SetDepth(const TFlatTexture &texture) {
        Set(Shader->Depth, texture);
        return std::move(*this);
    }

    THdrSetup&& SetBloom(const TFlatTexture& texture) {
        Set(Shader->Bloom, texture);
        return std::move(*this);
    }

    THdrSetup&& SetExposure(float exposure) {
        Set(Shader->Exposure, exposure);
        return std::move(*this);
    }
};
