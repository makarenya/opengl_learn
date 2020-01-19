#pragma once
#include <glm/glm.hpp>
#include "resource.h"

class TBlurShader: public TShaderProgram {
private:
    GLint Screen;
    GLint Depth;
public:
    TBlurShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&shaders_border_vert, shaders_border_vert_len)
            .SetFragment(&shaders_blur_frag, shaders_blur_frag_len))
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
