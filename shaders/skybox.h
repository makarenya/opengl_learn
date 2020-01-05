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
        return Program.Use()
                      .Texture("material.skybox", GL_TEXTURE_CUBE_MAP)
                      .Set("projection", projection)
                      .Set("view", glm::mat4(glm::mat3(view)));
    }
};
