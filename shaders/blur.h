#pragma once
#include <glm/glm.hpp>

class TBlurShader: public TShaderProgram {
public:
    TBlurShader()
        : TShaderProgram("shaders/border.vert", "shaders/blur.frag") {
    }
};

class TBlurSetup: public TProgramSetup {
public:
    TBlurSetup(const TBlurShader &shader)
        : TProgramSetup(shader) {
        Texture("screenTexture", GL_TEXTURE_2D);
        Texture("depthTexture", GL_TEXTURE_2D);
    }
};

