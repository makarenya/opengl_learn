#pragma once
#include "common.h"
#include <map>
#include <utility>
#include <variant>
#include "texture.h"
#include "mesh.h"

class TMaterial;

enum EMaterialProp {
    Ambient,
    Diffuse,
    Specular,
    Shininess,
    Reflection,
    Refraction,
    MATERIAL_PROPS_COUNT
};

using TMaterialTexture = std::variant<bool, TFlatTexture, TCubeTexture>;

class IMaterialBound {
public:
    virtual ~IMaterialBound() = default;
    virtual void SetTexture(EMaterialProp, const TMaterialTexture &texture) = 0;
    virtual void SetColor(EMaterialProp, glm::vec4) = 0;
    virtual void SetConstant(EMaterialProp, float value) = 0;
};

class TMaterialBuilder {
public:
    BUILDER_MAP(EMaterialProp, TMaterialTexture, Texture){};
    BUILDER_MAP(EMaterialProp, glm::vec4, Color){};
    BUILDER_MAP(EMaterialProp, float, Constant){};
};

class TMaterial {
private:
    std::array<TMaterialTexture, EMaterialProp::MATERIAL_PROPS_COUNT> Textures{};
    std::array<glm::vec4, EMaterialProp::MATERIAL_PROPS_COUNT> Colors{};
    std::array<float, EMaterialProp::MATERIAL_PROPS_COUNT> Constants{};

public:
    TMaterial(const TMaterialBuilder &builder);
    void DrawWith(IMaterialBound &bound, const TMesh &mesh) const;

    [[nodiscard]] TMaterialTexture GetTexture(EMaterialProp prop) const {
        return Textures[static_cast<int>(prop)];
    }

    [[nodiscard]] glm::vec4 GetColor(EMaterialProp prop) const {
        return Colors[static_cast<int>(prop)];
    }

    [[nodiscard]] float GetConstant(EMaterialProp prop) const {
        return Constants[static_cast<int>(prop)];
    }

    friend class TMaterialBinder;
};

class TMaterialBinder {
private:
    const TMaterial &Material;
    IMaterialBound &Shader;

public:
    TMaterialBinder(const TMaterial &material, IMaterialBound &shader);
    ~TMaterialBinder();
};