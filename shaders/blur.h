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

class TBlurSetup: public TShaderSetup, public IFrameBufferShader {
private:
    const TBlurShader *Shader;

public:
    explicit TBlurSetup(const TBlurShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    void SetScreen(const TTexture &texture) override {
        Set(Shader->Screen, texture);
    }

    void SetDepth(const TTexture &texture) override {
        Set(Shader->Depth, texture);
    }
};
