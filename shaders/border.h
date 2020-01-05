#pragma once
#include <glm/glm.hpp>

class TBorderSetup {
public:
    TProgramSetup Setup;

    TBorderSetup(TProgramSetup &&setup)
        : Setup(std::move(setup)) {
    }

    TBorderSetup &&Color(glm::vec4 color) {
        Setup.Set("borderColor", color);
        return std::move(*this);
    }
};

class TBorderShader {
    TShaderProgram Program;
public:
    TBorderShader()
        : Program("shaders/border.vert", "shaders/border.frag") {
    }

    TBorderSetup Use() {
        return {Program.Use()};
    }
};
