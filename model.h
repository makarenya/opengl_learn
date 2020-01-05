#pragma once
#include "mesh.h"
#include "material.h"
#include <vector>
#include <glm/glm.hpp>

class TModel {
private:
    std::vector<std::tuple<TMesh, int>> Meshes;
    std::vector<TMaterial> Materials;

public:
    TModel() = default;
    TModel(TMaterialBuilder&& materialBuilder, TMeshBuilder &&meshBuilder) {
        Materials.emplace_back(std::move(materialBuilder));
        Meshes.emplace_back(std::forward_as_tuple(std::move(meshBuilder), 0));
    }

    TModel &&Material(TMaterialBuilder &&mat) {
        Materials.emplace_back(std::move(mat));
        return std::move(*this);
    }

    TModel &&Mesh(TMeshBuilder &&builder, int material) {
        Meshes.emplace_back(std::forward_as_tuple(std::move(builder), material));
        return std::move(*this);
    }

    void Draw(TProgramSetup &setup) const {
        for (auto &mesh : Meshes) {
            auto binder = Materials[std::get<1>(mesh)].Bind(setup);
            std::get<0>(mesh).Draw();
        }
    }
};
