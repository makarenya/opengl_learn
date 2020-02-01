#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"
#include <iostream>

class TBlurShader: public TShaderProgram {
private:
    GLint Screen;
    GLint Vertical;
    GLint Size;
    GLint Kernel;
    GLint Threshold;
public:
    TBlurShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_border_vert)
            .SetFragment(&NResource::shaders_blur_frag))
          , Screen(DefineTexture("screenTexture"))
          , Vertical(DefineProp("vertical"))
          , Size(DefineProp("size"))
          , Kernel(DefineProp("kernel"))
          , Threshold(DefineProp("threshold")) {
    }
    friend class TBlurSetup;
};

class TBlurSetup: public TShaderSetup {
private:
    const TBlurShader *Shader;

public:
    explicit TBlurSetup(const TBlurShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TBlurSetup(TBlurSetup&& src) 
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TBlurSetup() {
        if (Shader != nullptr) {
            try {

            } catch (...) {
                Set(Shader->Vertical, false);
                Set(Shader->Size, 0);
                Set(Shader->Threshold, 0.0f);
            }
        }
    }

    TBlurSetup &&SetScreen(const TFlatTexture &texture) {
        Set(Shader->Screen, texture);
        return std::move(*this);
    }

    TBlurSetup&& SetVertical(bool vertical) {
        Set(Shader->Vertical, vertical);
        return std::move(*this);
    }

    TBlurSetup&& SetSigma(float sigma) {
        int size = sigma * 3;
        if (size >= 16) throw TGlBaseError("sigma too big");
        std::array<float, 16> kernel;
        float total = 0.0f;
        for (int i = 0; i <= size; i++) {
            kernel[i] = 1.0 / (sigma * std::sqrt(2 * M_PI)) * std::exp(-(i * i) / (2 * sigma * sigma));
            total += kernel[i];
        }
        for (int i = 0; i <= size; i++) {
            kernel[i] = kernel[i] / total;
        }

        Set(Shader->Kernel, kernel.data(), 16);
        Set(Shader->Size, size);
        return std::move(*this);
    }

    TBlurSetup&& SetThreshold(float threshold) {
        Set(Shader->Threshold, threshold);
        return std::move(*this);
    }
};
