#include "model.h"
#include "errors.h"
#include <vector>
#include <deque>
#include <array>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <algorithm>

using namespace std;
using namespace glm;

bool LoadTexture(const aiMaterial *material,
                 const std::string &name,
                 aiTextureType type,
                 const std::string &directory,
                 TMaterialBuilder &builder) {
    if (material->GetTextureCount(type) > 0) {
        aiString path;
        material->GetTexture(type, 0, &path);
        std::string texPath = directory + "/" + path.C_Str();
        builder.SetTexture(name, TTextureBuilder().SetFile(texPath));
        return true;
    }
    return false;
}

TModel LoadMesh(const std::string &filename) {
    std::array<char, PATH_MAX> real{};
    realpath(filename.c_str(), real.data());
    std::string fullpath(real.data());
    size_t pos = fullpath.rfind('/');
    std::string directory = fullpath.substr(0, pos);

    Assimp::Importer importer;
    auto scene = importer.ReadFile(fullpath, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || !scene->mRootNode || scene->mFlags & static_cast<unsigned>(AI_SCENE_FLAGS_INCOMPLETE)) {
        throw TGlBaseError("Can't import scene");
    }

    TModel model;

    for (unsigned i = 0; i < scene->mNumMaterials; i++) {
        TMaterialBuilder builder;
        auto material = scene->mMaterials[i];
        if (!LoadTexture(material, "diffuse_map", aiTextureType_DIFFUSE, directory, builder)) {
            aiColor3D color;
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
                builder.SetColor("diffuse_col", vec4(color.r, color.g, color.b, 1.0f));
            }
        }
        if (!LoadTexture(material, "specular_map", aiTextureType_SPECULAR, directory, builder)) {
            aiColor3D color;
            if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
                builder.SetColor("specular_col", vec4(color.r, color.g, color.b, 1.0f));
            }
        }
        //LoadTexture(material, "normals", aiTextureType_NORMALS, directory, dest);
        //LoadTexture(material, "height", aiTextureType_HEIGHT, directory, dest);
        ai_real shiness;
        if (material->Get(AI_MATKEY_SHININESS, shiness) == aiReturn_SUCCESS) {
            builder.SetConstant("shiness", shiness);
        }
        model.Material(std::move(builder));
    }

    vector<GLfloat> vertices;
    vector<GLuint> indexes;
    for (deque<aiNode *> nodes{scene->mRootNode}; !nodes.empty(); nodes.pop_back()) {
        auto node = nodes.back();
        for (unsigned i = 0; i < node->mNumChildren; ++i) {
            nodes.push_front(node->mChildren[i]);
        }
        for (unsigned i = 0; i < node->mNumMeshes; ++i) {
            auto mesh = scene->mMeshes[node->mMeshes[i]];
            auto material = mesh->mMaterialIndex;

            vertices.resize(mesh->mNumVertices * 8);
            indexes.resize(mesh->mNumFaces * 3);

            int vi = 0;
            for (unsigned j = 0; j < mesh->mNumVertices; j++) {
                auto vertex = mesh->mVertices[j];
                auto norm = mesh->mNormals[j];
                auto tex = mesh->mTextureCoords[0][j];
                vertices[vi++] = vertex.x;
                vertices[vi++] = vertex.y;
                vertices[vi++] = vertex.z;
                vertices[vi++] = norm.x;
                vertices[vi++] = norm.y;
                vertices[vi++] = norm.z;
                vertices[vi++] = tex.x;
                vertices[vi++] = tex.y;
                //mmin = vec3{std::min(mmin.x, vertex.x), std::min(mmin.y, vertex.y), std::min(mmin.z, vertex.z)};
                //mmax = vec3{std::max(mmax.x, vertex.x), std::max(mmax.y, vertex.y), std::max(mmax.z, vertex.z)};
            }
            int ii = 0;
            for (unsigned j = 0; j < mesh->mNumFaces; ++j) {
                auto face = mesh->mFaces[j];
                for (unsigned k = 0; k < face.mNumIndices; ++k) {
                    indexes[ii++] = face.mIndices[k];
                }
            }

            model.Mesh(
                mesh->mName.C_Str(),
                TMeshBuilder()
                    .SetVertices(EBufferUsage::Static, vertices)
                    .SetIndices(EBufferUsage::Static, indexes)
                    .AddLayout(EDataType::Float, 3)
                    .AddLayout(EDataType::Float, 3)
                    .AddLayout(EDataType::Float, 2),
                material);
        }
    }
    return model;
}

