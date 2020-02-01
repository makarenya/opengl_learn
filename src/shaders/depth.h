#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"

class TDepthShader: public TShaderProgram {
private:
    GLint Depth;

public:
    TDepthShader()
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_border_vert)
            .SetFragment(&NResource::shaders_depth_frag))
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

    TDepthSetup &&SetDepth(const TCubeTexture &texture) {
        Set(Shader->Depth, texture);
        return std::move(*this);
    }
};
