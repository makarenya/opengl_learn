#pragma once
#include <glm/glm.hpp>

class TLightShader: public TShaderProgram {
public:
    TLightShader()
        : TShaderProgram("shaders/light.vert", "shaders/light.frag") {
    }
};

class TLightSetup: public TProgramSetup {
public:
    TLightSetup(const TLightShader &shader, glm::mat4 projection, glm::mat4 view)
        : TProgramSetup(shader) {
        Set("projection", projection);
        Set("view", view);
    }

    TLightSetup &&Model(glm::mat4 model) {
        Set("model", model);
        return std::move(*this);
    }

    TLightSetup &&Color(glm::vec3 color) {
        Set("lightColor", color);
        return std::move(*this);
    }
};
