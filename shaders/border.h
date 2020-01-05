#pragma once
#include <glm/glm.hpp>

class TBorderShader: public TShaderProgram {
public:
    TBorderShader()
        : TShaderProgram("shaders/border.vert", "shaders/border.frag") {
    }
};

class TBorderSetup: public TProgramSetup {
public:
    TBorderSetup(const TBorderShader &shader)
        : TProgramSetup(shader) {
        Texture("screenTexture", GL_TEXTURE_2D);
        Texture("depthTexture", GL_TEXTURE_2D);
    }

    TBorderSetup &&Color(glm::vec4 color) {
        Set("borderColor", color);
        return std::move(*this);
    }
};
