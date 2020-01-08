#pragma once
#include <glm/glm.hpp>
#include "../framebuffer.h"

class TBorderShader: public TShaderProgram {
    int Screen;
    int Depth;
    GLint Color;
public:
    TBorderShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex("shaders/border.vert")
            .SetFragment("shaders/border.frag"))
          , Screen(DefineTexture("screenTexture"))
          , Depth(DefineTexture("depthTexture"))
          , Color(DefineProp("borderColor")) {
    }
    friend class TBorderSetup;
};

class TBorderSetup: public TShaderSetup, public IFrameBufferShader {
private:
    const TBorderShader *Shader;

public:
    explicit TBorderSetup(const TBorderShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TBorderSetup(TBorderSetup &&src) noexcept
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TBorderSetup() override {
        if (Shader != nullptr) {
            try {
                Set(Shader->Color, glm::vec4(0));
            } catch (...) {
            }
        }
    }

    TBorderSetup &SetColor(glm::vec4 color) {
        Set(Shader->Color, color);
        return *this;
    }

    void SetScreen(const TTexture &texture) override {
        Set(Shader->Screen, texture);
    }

    void SetDepth(const TTexture &texture) override {
        Set(Shader->Depth, texture);
    }
};
