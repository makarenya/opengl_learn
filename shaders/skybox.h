#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"

class TSkyboxShader: public TShaderProgram {
public:
    TSkyboxShader()
        : TShaderProgram("shaders/skybox.vert", "shaders/skybox.frag") {
    }
};

class TSkyboxSetup: public TProgramSetup {
public:
    TSkyboxSetup(const TSkyboxShader &shader, glm::mat4 projection, glm::mat4 view)
        : TProgramSetup(shader) {
        Texture("material.skybox", GL_TEXTURE_CUBE_MAP);
        Set("projection", projection);
        Set("view", glm::mat4(glm::mat3(view)));
    }
};


