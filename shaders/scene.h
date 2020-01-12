#pragma once
#include <glm/glm.hpp>

class TSceneShader: public TShaderProgram {
private:
    GLint Model;
    GLint SkyBox;
    GLint Shadow;
    GLint SpotShadow;
    GLint SpotShadow2;
    GLint LightMatrix;
    GLint ViewPos;
    GLint Explosion;
    GLint Opaque;
public:
    TSceneShader(const TUniformBindingBase &matrices, const TUniformBindingBase &lights)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex("shaders/scene.vert")
            .SetFragment("shaders/scene.frag")
            .SetGeometry("shaders/scene.geom")
            .SetBlock("Matrices", matrices)
            .SetBlock("Lights", lights)
            .SetTexture("material.diffuse_map", EMaterialProp::Diffuse)
            .SetTexture("material.specular_map", EMaterialProp::Specular)
            .SetTexture("material.shiness_map", EMaterialProp::Shininess)
            .SetColor("material.diffuse_col", EMaterialProp::Diffuse)
            .SetColor("material.specular_col", EMaterialProp::Specular)
            .SetConstant("material.shiness", EMaterialProp::Shininess)
            .SetConstant("material.reflection", EMaterialProp::Reflection)
            .SetConstant("material.refraction", EMaterialProp::Refraction))
          , Model(DefineProp("model"))
          , SkyBox(DefineTexture("skybox"))
          , Shadow(DefineTexture("shadow"))
          , SpotShadow(DefineTexture("spotShadow"))
          , SpotShadow2(DefineTexture("spotShadow2"))
          , LightMatrix(DefineProp("lightMatrix"))
          , ViewPos(DefineProp("viewPos"))
          , Explosion(DefineProp("explosion"))
          , Opaque(DefineProp("opaque")) {
    }

    friend class TSceneSetup;
};

class TSceneSetup: public TShaderSetup {
private:
    const TSceneShader *Shader;

public:
    explicit TSceneSetup(const TSceneShader *shader) : TShaderSetup(shader), Shader(shader) {
    }

    TSceneSetup(TSceneSetup &&src) noexcept
        : TShaderSetup(std::move(src))
          , Shader(src.Shader) {
        src.Shader = nullptr;
    }

    ~TSceneSetup() override {
        if (Shader != nullptr) {
            try {
                Set(Shader->Model, glm::mat4(0));
                Set(Shader->ViewPos, glm::vec3(0));
                Set(Shader->Explosion, 0.0f);
                Set(Shader->Opaque, false);
            } catch (...) {
            }
        }
    }

    TSceneSetup &&SetModel(glm::mat4 model) {
        Set(Shader->Model, model);
        return std::move(*this);
    }

    TSceneSetup &&SetViewPos(glm::vec3 position) {
        Set(Shader->ViewPos, position);
        return std::move(*this);
    }

    TSceneSetup &&SetExplosion(float value) {
        Set(Shader->Explosion, value);
        return std::move(*this);
    }

    TSceneSetup &&SetOpaque(bool value) {
        Set(Shader->Opaque, value);
        return std::move(*this);
    }

    TSceneSetup &&SetShadow(TFlatTexture &texture) {
        Set(Shader->Shadow, texture);
        return std::move(*this);
    }

    TSceneSetup &&SetSpotShadow(TCubeTexture &texture) {
        Set(Shader->SpotShadow, texture);
        return std::move(*this);
    }

    TSceneSetup &&SetSpotShadow2(TCubeTexture &texture) {
        Set(Shader->SpotShadow2, texture);
        return std::move(*this);
    }

    TSceneSetup &&SetLightMatrix(glm::mat4 matrix) {
        Set(Shader->LightMatrix, matrix);
        return std::move(*this);
    }

    TSceneSetup &&SetSkyBox(TCubeTexture &texture) {
        Set(Shader->SkyBox, texture);
        return std::move(*this);
    }
};
