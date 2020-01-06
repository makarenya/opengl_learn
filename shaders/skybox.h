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
    TSkyboxSetup(const TSkyboxShader &shader)
        : TProgramSetup(shader) {
        Texture("material.skybox", GL_TEXTURE_CUBE_MAP);
    }
};


