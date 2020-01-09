#include <iostream>
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

bool LoadColorTexture(const aiMaterial *material,
                      EMaterialProp prop,
                      aiTextureType type,
                      const char *colorKey,
                      unsigned colorType,
                      unsigned colorIndex,
                      const std::string &directory,
                      TMaterialBuilder &builder) {
    if (material->GetTextureCount(type) > 0) {
        aiString path;
        material->GetTexture(type, 0, &path);
        std::string texPath = directory + "/" + path.C_Str();
        builder.SetTexture(prop, TTextureBuilder().SetFile(texPath).SetUsage(ETextureUsage::SRgba));
        return true;
    }
    aiColor3D color;
    if (material->Get(colorKey, colorType, colorIndex, color) == aiReturn_SUCCESS) {
        builder.SetColor(prop, vec4(color.r, color.g, color.b, 1.0f));
        return true;
    }
    return false;
}

bool LoadConstantTexture(const aiMaterial *material,
                         EMaterialProp prop,
                         aiTextureType type,
                         const char *constantKey,
                         unsigned constantType,
                         unsigned constantIndex,
                         const std::string &directory,
                         TMaterialBuilder &builder) {
    if (material->GetTextureCount(type) > 0) {
        aiString path;
        material->GetTexture(type, 0, &path);
        std::string texPath = directory + "/" + path.C_Str();
        builder.SetTexture(prop, TTextureBuilder().SetFile(texPath));
        return true;
    }
    float constant;
    if (material->Get(constantKey, constantType, constantIndex, constant) == aiReturn_SUCCESS) {
        builder.SetConstant(prop, constant);
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
        LoadColorTexture(material, EMaterialProp::Diffuse, aiTextureType_DIFFUSE,
                         AI_MATKEY_COLOR_DIFFUSE, directory, builder);
        LoadColorTexture(material, EMaterialProp::Specular, aiTextureType_SPECULAR,
                         AI_MATKEY_COLOR_SPECULAR, directory, builder);
        LoadConstantTexture(material, EMaterialProp::Shininess, aiTextureType_SHININESS,
                            AI_MATKEY_SHININESS, directory, builder);
        LoadConstantTexture(material, EMaterialProp::Reflection, aiTextureType_REFLECTION,
                            AI_MATKEY_REFLECTIVITY, directory, builder);

        model.Material(builder);
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
                //cout << vertex.x << ", " << vertex.y << ", " << vertex.z << std::endl;
            }
            int ii = 0;
            for (unsigned j = 0; j < mesh->mNumFaces; ++j) {
                auto face = mesh->mFaces[j];
                if (face.mNumIndices != 3) {
                    throw TGlBaseError("invalid file format");
                }
                for (unsigned k = 0; k < face.mNumIndices; ++k) {
                    indexes[ii++] = face.mIndices[k];
                }
            }
            model.Mesh(mesh->mName.C_Str(),
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

