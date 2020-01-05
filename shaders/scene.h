#pragma once
#include <glm/glm.hpp>

class TSceneSetup {
private:
    int SpotCount{};

public:
    TProgramSetup Setup;

    TSceneSetup(TProgramSetup &&setup)
        : Setup(std::move(setup)) {
    }

    TSceneSetup(TSceneSetup &&setup)
        : Setup(std::move(setup.Setup)) {
    }

    TSceneSetup(const TSceneSetup &) = delete;
    TSceneSetup &operator=(const TSceneSetup &) = delete;


    TSceneSetup &&Model(glm::mat4 model) {
        Setup.Set("model", model);
        return std::move(*this);
    }

    TSceneSetup &&CanDiscard(bool canDiscard) {
        Setup.Set("canDiscard", canDiscard);
        return std::move(*this);
    }

    TSceneSetup &&DirectionalLight(glm::vec3 direction,
                                   glm::vec3 ambient,
                                   glm::vec3 diffuse,
                                   glm::vec3 specular) {
        Setup.Set("directional.direction", direction)
             .Set("directional.ambient", ambient)
             .Set("directional.diffuse", diffuse)
             .Set("directional.specular", specular);
        return std::move(*this);
    }

    TSceneSetup &&ProjectorLight(glm::vec3 position,
                                 glm::vec3 target,
                                 glm::vec3 ambient,
                                 glm::vec3 diffuse,
                                 glm::vec3 specular,
                                 float innerCutoff,
                                 float outerCutoff) {
        Setup.Set("projector.position", position)
             .Set("projector.target", target)
             .Set("projector.ambient", ambient)
             .Set("projector.diffuse", diffuse)
             .Set("projector.specular", specular)
             .Set("projector.innerCutoff", cos(glm::radians(innerCutoff)))
             .Set("projector.outerCutoff", cos(glm::radians(outerCutoff)));
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
        Setup.Set(prefix + "position", position)
             .Set(prefix + "ambient", ambient)
             .Set(prefix + "diffuse", diffuse)
             .Set(prefix + "specular", specular)
             .Set(prefix + "linear", linear)
             .Set(prefix + "quadratic", quadratic)
             .Set("spotCount", SpotCount);
        return std::move(*this);
    }
};

class TSceneShader {
    TShaderProgram Program;
public:
    TSceneShader()
        : Program("shaders/scene.vert", "shaders/scene.frag") {
    }

    TSceneSetup Use(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos) {
        return {Program.Use().Set("projection", projection)
                       .Set("view", view)
                       .Set("viewPos", viewPos)};
    }
};
