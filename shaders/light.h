#pragma once
#include <glm/glm.hpp>

class TLightSetup {
public:
    TProgramSetup Setup;

    TLightSetup(TProgramSetup &&setup) : Setup(std::move(setup)) {
    }

    TLightSetup &&Model(glm::mat4 model) {
        Setup.Set("model", model);
        return std::move(*this);
    }

    TLightSetup &&Color(glm::vec3 color) {
        Setup.Set("lightColor", color);
        return std::move(*this);
    }
};

class TLightShader {
    TShaderProgram Program;
public:
    TLightShader()
        : Program("shaders/light.vert", "shaders/light.frag") {
    }

    TLightSetup Use(glm::mat4 projection, glm::mat4 view) {
        return {Program.Use().Set("projection", projection).Set("view", view)};
    }
};
