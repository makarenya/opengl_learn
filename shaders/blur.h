#pragma once
#include <glm/glm.hpp>

class TBlurSetup {
public:
    TProgramSetup Setup;

    TBlurSetup(TProgramSetup &&setup)
        : Setup(std::move(setup)) {
    }
};

class TBlurShader {
    TShaderProgram Program;
public:
    TBlurShader()
        : Program("shaders/border.vert", "shaders/blur.frag") {
    }

    TBlurSetup Use() {
        return {Program.Use()
                       .Texture("screenTexture", GL_TEXTURE_2D)
                       .Texture("depthTexture", GL_TEXTURE_2D)};
    }
};
