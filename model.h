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

    TModel &Material(const TMaterialBuilder &mat) {
        Materials.emplace_back(mat);
        return *this;
    }

    TModel &Mesh(const std::string &name, const TMeshBuilder &builder, int material) {
        Meshes.emplace_back(std::forward_as_tuple(builder, name, material));
        return *this;
    }

    void Draw(TProgramSetup &setup) const {
        for (auto&[mesh, name, mat] : Meshes) {
            Materials[mat].Use(setup);
            mesh.Draw();
        }
    }

    void Draw(TProgramSetup &setup,
              const std::function<void(const std::string &, const TMesh &)> &fn) const {
        for (auto&[mesh, name, mat] : Meshes) {
            Materials[mat].Use(setup);
            fn(name, mesh);
        }
    }

    TMesh &GetMesh(int index) {
        return std::get<0>(Meshes[index]);
    }
};
