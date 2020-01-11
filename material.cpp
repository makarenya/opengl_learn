#include "material.h"

TMaterial::TMaterial(const TMaterialBuilder &builder) {
    for (auto&[key, value]: builder.Textures_) {
        Textures[static_cast<int>(key)] = value;
    }
    for (auto&[key, value]: builder.Colors_) {
        Colors[static_cast<int>(key)] = value;
    }
    for (auto&[key, value]: builder.Constants_) {
        Constants[static_cast<int>(key)] = value;
    }
}

void TMaterial::DrawWith(IMaterialBound &bound, const TMesh &mesh) const {
    TMaterialBinder binder(*this, bound);
    mesh.Draw();
}

TMaterialBinder::TMaterialBinder(const TMaterial &material, IMaterialBound &shader)
    : Material(material)
      , Shader(shader) {
    for (int i = 0; i < material.Textures.size(); ++i) {
        if (material.Textures[i].index() != 0) {
            shader.SetTexture(static_cast<EMaterialProp>(i), material.Textures[i]);
        }
    }
    for (int i = 0; i < material.Colors.size(); ++i) {
        if (material.Colors[i] != glm::vec4(0.0f)) {
            shader.SetColor(static_cast<EMaterialProp>(i), material.Colors[i]);
        }
    }
    for (int i = 0; i < material.Constants.size(); ++i) {
        if (material.Constants[i] != 0.0f) {
            shader.SetConstant(static_cast<EMaterialProp>(i), material.Constants[i]);
        }
    }
}

TMaterialBinder::~TMaterialBinder() {
    for (int i = 0; i < Material.Textures.size(); ++i) {
        if (Material.Textures[i].index() != 0) {
            Shader.SetTexture(static_cast<EMaterialProp>(i), TMaterialTexture());
        }
    }
    for (int i = 0; i < Material.Colors.size(); ++i) {
        if (Material.Colors[i] != glm::vec4(0.0f)) {
            Shader.SetColor(static_cast<EMaterialProp>(i), glm::vec4(0.0f));
        }
    }
    for (int i = 0; i < Material.Constants.size(); ++i) {
        if (Material.Constants[i] != 0.0f) {
            Shader.SetConstant(static_cast<EMaterialProp>(i), 0.0f);
        }
    }
}
