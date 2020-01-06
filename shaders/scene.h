#pragma once
#include <glm/glm.hpp>

class TSceneShader: public TShaderProgram {
public:
    TSceneShader()
        : TShaderProgram("shaders/scene.vert", "shaders/scene.frag") {
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

    TSceneSetup &&DirectionalLight(glm::vec3 direction,
                                   glm::vec3 ambient,
                                   glm::vec3 diffuse,
                                   glm::vec3 specular) {
        Set("directional.direction", direction);
        Set("directional.ambient", ambient);
        Set("directional.diffuse", diffuse);
        Set("directional.specular", specular);
        return std::move(*this);
    }

    TSceneSetup &&ProjectorLight(glm::vec3 position,
                                 glm::vec3 target,
                                 glm::vec3 ambient,
                                 glm::vec3 diffuse,
                                 glm::vec3 specular,
                                 float innerCutoff,
                                 float outerCutoff) {
        Set("projector.position", position);
        Set("projector.target", target);
        Set("projector.ambient", ambient);
        Set("projector.diffuse", diffuse);
        Set("projector.specular", specular);
        Set("projector.innerCutoff", cos(glm::radians(innerCutoff)));
        Set("projector.outerCutoff", cos(glm::radians(outerCutoff)));
        return std::move(*this);
    }

    TSceneSetup &&SpotLight(glm::vec3 position,
                            glm::vec3 ambient,
                            glm::vec3 diffuse,
                            glm::vec3 specular,
                            float linear,
                            float quadratic) {
        const int index = SpotCount++;
        std::string prefix = "spots[" + std::to_string(index) + "].";
        Set(prefix + "position", position);
        Set(prefix + "ambient", ambient);
        Set(prefix + "diffuse", diffuse);
        Set(prefix + "specular", specular);
        Set(prefix + "linear", linear);
        Set(prefix + "quadratic", quadratic);
        Set("spotCount", SpotCount);
        return std::move(*this);
    }
};

