#pragma once
#include <glm/glm.hpp>
#include "../shader_program.h"
#include "../resource.h"

class TSceneShader: public TShaderProgram {
private:
    GLint Model;
    GLint NormModel;
    GLint SkyBox;
    GLint Shadow;
    GLint SpotShadow;
    GLint SpotShadow2;
    GLint LightMatrix;
    GLint ViewPos;
    GLint Explosion;
    GLint Opaque;
    GLint UseMap;
public:
    TSceneShader(const TUniformBindingBase &matrices, const TUniformBindingBase &lights, const TUniformBindingBase &lightsPos)
        : TShaderProgram(
        TShaderBuilder()
            .SetVertex(&NResource::shaders_scene_vert)
            .SetFragment(&NResource::shaders_scene_frag)
            .SetGeometry(&NResource::shaders_scene_geom)
            .SetBlock("Matrices", matrices)
            .SetBlock("Lights", lights)
            .SetBlock("LightsPos", lightsPos)
            .SetTexture(EMaterialProp::Diffuse, "material.diffuse_map", "material.has_diffuse_map")
            .SetTexture(EMaterialProp::Specular, "material.specular_map", "material.has_specular_map")
            .SetTexture(EMaterialProp::Shininess, "material.shiness_map", "material.has_shiness_map")
            .SetTexture(EMaterialProp::Normal, "material.normal_map", "material.has_normal_map")
            .SetTexture(EMaterialProp::Height, "material.height_map", "material.has_height_map")
            .SetColor(EMaterialProp::Diffuse, "material.diffuse_col")
            .SetColor(EMaterialProp::Specular, "material.specular_col")
            .SetConstant(EMaterialProp::Shininess, "material.shiness")
            .SetConstant(EMaterialProp::Reflection, "material.reflection")
            .SetConstant(EMaterialProp::Refraction, "material.refraction"))
          , Model(DefineProp("model"))
          , NormModel(DefineProp("normModel", true))
          , SkyBox(DefineTexture("skybox"))
          , Shadow(DefineTexture("shadow"))
          , SpotShadow(DefineTexture("spotShadow"))
          , SpotShadow2(DefineTexture("spotShadow2"))
          , LightMatrix(DefineProp("lightMatrix"))
          , ViewPos(DefineProp("viewPos"))
          , Explosion(DefineProp("explosion"))
          , Opaque(DefineProp("opaque"))
          , UseMap(DefineProp("useMap")) {
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
        if (Shader->NormModel >= 0) {
            Set(Shader->NormModel, glm::transpose(glm::inverse(glm::mat3(model))));
        }
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

    TSceneSetup &&SetUseMap(bool useMap) {
        Set(Shader->UseMap, useMap);
        return std::move(*this);
    }
};
