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
    TSceneShader(const TUniformBindingBase &matrices, const TUniformBindingBase &lights, const TUniformBindingBase &lightsPos)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex("shaders/scene.vert")
            .SetFragment("shaders/scene.frag")
            .SetGeometry("shaders/scene.geom")
            .SetBlock("Matrices", matrices)
            .SetBlock("Lights", lights)
            .SetBlock("LightsPos", lightsPos)
            .SetTexture(EMaterialProp::Diffuse, "material.diffuse_map", "material.has_diffuse_map")
            .SetTexture(EMaterialProp::Specular, "material.specular_map", "material.has_specular_map")
            .SetTexture(EMaterialProp::Shininess, "material.shiness_map", "material.has_shiness_map")
            .SetTexture(EMaterialProp::Normal, "material.normal_map", "material.has_normal_map")
            .SetColor(EMaterialProp::Diffuse, "material.diffuse_col")
            .SetColor(EMaterialProp::Specular, "material.specular_col")
            .SetConstant(EMaterialProp::Shininess, "material.shiness")
            .SetConstant(EMaterialProp::Reflection, "material.reflection")
            .SetConstant(EMaterialProp::Refraction, "material.refraction"))
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
