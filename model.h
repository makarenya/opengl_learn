#pragma once
#include "mesh.h"
#include "material.h"
#include "shader_program.h"

class TModel {
private:
    std::vector<std::tuple<TMesh, std::string, size_t>> Meshes;
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

    void Draw(TShaderSetup &&setup) const {
        for (auto&[mesh, name, mat] : Meshes) {
            TMaterialBinder binder(Materials[mat], setup);
            mesh.Draw();
        }
    }

    void Draw(TShaderSetup &setup) const {
        for (auto&[mesh, name, mat] : Meshes) {
            TMaterialBinder binder(Materials[mat], setup);
            mesh.Draw();
        }
    }

    void Draw(TShaderSetup &setup,
              const std::function<void(const std::string &, const TMesh &, const TMaterial &material)> &fn) const {
        for (auto&[mesh, name, mat] : Meshes) {
            fn(name, mesh, Materials.at(mat));
        }
    }

    [[nodiscard]] size_t MeshCount() const {
        return Meshes.size();
    }

    [[nodiscard]] const TMesh &GetMesh(int index) const {
        return std::get<0>(Meshes.at(index));
    }

    [[nodiscard]] size_t MaterialsCount() const {
        return Materials.size();
    }

    [[nodiscard]] const TMaterial &GetMaterial(int index) const {
        return Materials.at(index);
    }

    [[nodiscard]] const TMaterial &GetMeshMaterial(int index) const {
        return Materials.at(std::get<2>(Meshes.at(index)));
    }
};
