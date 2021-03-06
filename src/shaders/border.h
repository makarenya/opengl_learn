#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"

class TBorderShader: public TShaderProgram {
    GLint Screen;
    GLint Depth;
    GLint Color;
    GLint Size;
    GLint Kernel;
    GLint Inner;

public:
    TBorderShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_border_vert)
            .SetFragment(&NResource::shaders_border_frag))
          , Screen(DefineTexture("screenTexture"))
          , Depth(DefineTexture("depthTexture"))
          , Color(DefineProp("borderColor"))
          , Size(DefineProp("size"))
          , Kernel(DefineProp("kernel"))
          , Inner(DefineProp("inner")) {
    }
    friend class TBorderSetup;
};

class TBorderSetup: public TShaderSetup {
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
                Set(Shader->Size, 0);
            } catch (...) {
            }
        }
    }

    TBorderSetup &&SetColor(glm::vec4 color) {
        Set(Shader->Color, color);
        return std::move(*this);
    }

    TBorderSetup &&SetKernel(float radius, float innerRadius = 0) {
        if (radius > 3.5) throw TGlBaseError("radius too big");
        std::array<GLfloat, 49> kernel{};
        std::array<GLfloat, 49> inner{};
        int size = static_cast<int>(std::ceil(radius - 0.5));
        int point = 0;
        int outerValue = static_cast<int>(radius * radius * 121);
        int innerValue = static_cast<int>(innerRadius * innerRadius * 121);
        for (int y = 0; y < 2 * size + 1; y++) {
            for (int x = 0; x < 2 * size + 1; x++) {
                int innerHits = 0;
                int outerHits = 0;
                for (int i = 0; i < 11; i++) {
                    for (int j = 0; j < 11; j++) {
                        int py = y * 11 + j - size * 11 - 5;
                        int px = x * 11 + i - size * 11 - 5;
                        int length = px * px + py * py;
                        if (length < outerValue) outerHits++;
                        if (length < innerValue) innerHits++;
                    }
                }
                kernel[point] = outerHits / 121.0;
                inner[point] = x == size && y == size ? 1.0 : innerHits / 121.0;
                point++;
            }
        }
        Set(Shader->Kernel, kernel.data(), 49);
        Set(Shader->Inner, inner.data(), 49);
        Set(Shader->Size, size);
        return std::move(*this);
    }

    TBorderSetup &&SetScreen(const TFlatTexture &texture) {
        Set(Shader->Screen, texture);
        return std::move(*this);
    }

    TBorderSetup &&SetDepth(const TFlatTexture &texture) {
        Set(Shader->Depth, texture);
        return std::move(*this);
    }
};
