#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"

class TBlurShader: public TShaderProgram {
private:
    GLint Screen;
    GLint Depth;
public:
    TBlurShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_border_vert)
            .SetFragment(&NResource::shaders_blur_frag))
          , Screen(DefineTexture("screenTexture"))
          , Depth(DefineTexture("depthTexture")) {
    }
    friend class TBlurSetup;
};

class TBlurSetup: public TShaderSetup {
private:
    const TBlurShader *Shader;

public:
    explicit TBlurSetup(const TBlurShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TBlurSetup &&SetScreen(const TFlatTexture &texture) {
        Set(Shader->Screen, texture);
        return std::move(*this);
    }

    TBlurSetup &&SetDepth(const TFlatTexture &texture) {
        Set(Shader->Depth, texture);
        return std::move(*this);
    }
};
