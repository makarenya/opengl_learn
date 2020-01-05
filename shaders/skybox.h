#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TSkyboxSetup {
public:
    TProgramSetup Setup;

    TSkyboxSetup(TProgramSetup &&setup)
        : Setup(std::move(setup)) {
    }
};

class TSkyboxShader {
    TShaderProgram Program;
public:
    TSkyboxShader()
        : Program("shaders/skybox.vert", "shaders/skybox.frag") {
    }

    TSkyboxSetup Use(glm::mat4 projection, glm::mat4 view) {
        auto setup = Program.Use();
        if (setup.Has("projection")) setup.Set("projection", projection);
        if (setup.Has("view")) setup.Set("view", glm::mat4(glm::mat3(view)));
        return setup;
    }
};
