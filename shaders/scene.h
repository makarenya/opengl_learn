#pragma once
#include <glm/glm.hpp>

class TSceneShader: public TShaderProgram {
public:
    TSceneShader()
        : TShaderProgram("shaders/scene.vert", "shaders/scene.frag", "shaders/scene.geom") {
    }
};

class TSceneSetup: public TProgramSetup {
private:
    int SpotCount{};

public:
    TSceneSetup(const TSceneShader &shader, glm::vec3 viewPos)
        : TProgramSetup(shader) {
        Texture("material.diffuse_map", GL_TEXTURE_2D);
        Texture("material.specular_map", GL_TEXTURE_2D);
        Texture("material.shiness_map", GL_TEXTURE_2D);
        Texture("material.skybox", GL_TEXTURE_CUBE_MAP);
        Set("viewPos", viewPos);
    }

    TSceneSetup(const TSceneSetup &) = delete;
    TSceneSetup &operator=(const TSceneSetup &) = delete;

    TSceneSetup &&Explosion(float value) {
        Set("explosion", value);
        return std::move(*this);
    }

    TSceneSetup &&Model(glm::mat4 model) {
        Set("model", model);
        return std::move(*this);
    }

    TSceneSetup &&CanDiscard(bool canDiscard) {
        Set("canDiscard", canDiscard);
        return std::move(*this);
    }

    TSceneSetup &&Reflection(float reflect) {
        Set("material.reflection", reflect);
        Set("material.refraction", 0.0f);
        return std::move(*this);
    }

    TSceneSetup &&Refraction(float refract) {
        Set("material.reflection", 0.0f);
        Set("material.refraction", refract);
        return std::move(*this);
    }

    int Skybox() {
        return TextureLoc("material.skybox");
    }

    TSceneSetup &&NoReflectRefract() {
        Set("material.reflection", 0.0f);
        Set("material.refraction", 0.0f);
        return std::move(*this);
    }
};

