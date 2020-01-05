#pragma once
#include "mesh.h"
#include "material.h"
#include <vector>
#include <glm/glm.hpp>

class TModel {
private:
    std::vector<std::tuple<TMesh, std::string, int>> Meshes;
    std::vector<TMaterial> Materials;

public:
    TModel() = default;
    TModel(const std::string &name, const TMaterialBuilder &materialBuilder, TMeshBuilder &&meshBuilder) {
        Materials.emplace_back(materialBuilder);
        Meshes.emplace_back(std::forward_as_tuple(std::move(meshBuilder), name, 0));
    }

    TModel &&Material(const TMaterialBuilder &mat) {
        Materials.emplace_back(mat);
        return std::move(*this);
    }

    TModel &&Mesh(const std::string &name, TMeshBuilder &&builder, int material) {
        Meshes.emplace_back(std::forward_as_tuple(std::move(builder), name, material));
        return std::move(*this);
    }

    void Draw(TProgramSetup &setup) const {
        for (auto&[mesh, name, mat] : Meshes) {
            auto binder = Materials[mat].Bind(setup);
            mesh.Draw();
        }
    }

    void Draw(TProgramSetup &setup,
              const std::function<void(const std::string &, const TMesh &, TTextureBinder &)> &fn) const {
        for (auto&[mesh, name, mat] : Meshes) {
            auto binder = Materials[mat].Bind(setup);
            fn(name, mesh, binder);
        }
    }
};
