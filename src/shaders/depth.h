#pragma once
#include <glm/glm.hpp>

class TDepthShader: public TShaderProgram {
private:
    GLint Depth;
    GLint Perspective;
public:
    TDepthShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&shaders_border_vert, shaders_border_vert_len)
            .SetFragment(&shaders_depth_frag, shaders_depth_frag_len))
          , Depth(DefineTexture("depthTexture")) {
    }
    friend class TDepthSetup;
};

class TDepthSetup: public TShaderSetup {
private:
    const TDepthShader *Shader;

public:
    explicit TDepthSetup(const TDepthShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TDepthSetup(TDepthSetup &&src) noexcept
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TDepthSetup() override {
        if (Shader != nullptr) {
            try {
                Set(Shader->Perspective, 0.0f, 0.0f);
            } catch (...) {
            }
        }
    }

    TDepthSetup &&SetDepth(const TCubeTexture &texture) {
        Set(Shader->Depth, texture);
        return std::move(*this);
    }
};
