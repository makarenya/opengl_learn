#pragma once
#include <glm/glm.hpp>

class TBlurShader: public TShaderProgram {
private:
    GLint Screen;
    GLint Depth;
public:
    TBlurShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex("shaders/border.vert")
            .SetFragment("shaders/blur.frag"))
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
