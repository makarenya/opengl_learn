#include "model.h"
#include "errors.h"
#include <vector>
#include <deque>
#include <array>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <algorithm>
#ifndef __APPLE__
#include <filesystem>
#endif
using namespace std;
using namespace glm;

bool LoadColorTexture(const aiMaterial *material,
                      EMaterialProp prop,
                      aiTextureType type,
                      const char *colorKey,
                      unsigned colorType,
                      unsigned colorIndex,
                      ETextureUsage usage,
                      const std::string &directory,
                      TMaterialBuilder &builder) {
    if (material->GetTextureCount(type) > 0) {
        aiString path;
        material->GetTexture(type, 0, &path);
        std::string texPath = directory + "/" + path.C_Str();
        builder.SetTexture(prop, TTextureBuilder().SetFile(texPath).SetUsage(usage));
        return true;
    }
    aiColor3D color;
    if (colorKey != nullptr && material->Get(colorKey, colorType, colorIndex, color) == aiReturn_SUCCESS) {
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
#ifdef __APPLE__
    std::array<char, PATH_MAX> real{};
    realpath(filename.c_str(), real.data());
    std::string fullpath(real.data());
    size_t pos = fullpath.rfind('/');
    std::string directory = fullpath.substr(0, pos);
#else
    auto canonical = std::filesystem::canonical(filename);
    auto fullpath = canonical.string();
    auto directory = canonical.parent_path().string();
#endif

    Assimp::Importer importer;
    auto scene = importer.ReadFile(fullpath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || !scene->mRootNode || scene->mFlags & static_cast<unsigned>(AI_SCENE_FLAGS_INCOMPLETE)) {
        throw TGlBaseError("Can't import scene");
    }

    TModel model;
    for (unsigned i = 0; i < scene->mNumMaterials; i++) {
        TMaterialBuilder builder;
        auto material = scene->mMaterials[i];
        LoadColorTexture(material, EMaterialProp::Diffuse, aiTextureType_DIFFUSE,
                         AI_MATKEY_COLOR_DIFFUSE, ETextureUsage::SRgba, directory, builder);
        LoadColorTexture(material, EMaterialProp::Specular, aiTextureType_SPECULAR,
                         AI_MATKEY_COLOR_SPECULAR, ETextureUsage::SRgba, directory, builder);
        LoadConstantTexture(material, EMaterialProp::Shininess, aiTextureType_SHININESS,
                            AI_MATKEY_SHININESS, directory, builder);
        LoadConstantTexture(material, EMaterialProp::Reflection, aiTextureType_REFLECTION,
                            AI_MATKEY_REFLECTIVITY, directory, builder);
        LoadColorTexture(material, EMaterialProp::Normal, aiTextureType_HEIGHT,
                         nullptr, 0, 0, ETextureUsage::Normals, directory, builder);
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

            vertices.resize(mesh->mNumVertices * 14);
            indexes.resize(mesh->mNumFaces * 3);

            int vi = 0;
            for (unsigned j = 0; j < mesh->mNumVertices; j++) {
                auto vertex = mesh->mVertices[j];
                auto norm = mesh->mNormals[j];
                auto tg = mesh->mTangents[j];
                auto bg = mesh->mBitangents[j];
                auto tex = mesh->mTextureCoords[0][j];
                vertices[vi++] = vertex.x;
                vertices[vi++] = vertex.y;
                vertices[vi++] = vertex.z;
                vertices[vi++] = norm.x;
                vertices[vi++] = norm.y;
                vertices[vi++] = norm.z;
                vertices[vi++] = tex.x;
                vertices[vi++] = tex.y;
                vertices[vi++] = tg.x;
                vertices[vi++] = tg.y;
                vertices[vi++] = tg.z;
                vertices[vi++] = bg.x;
                vertices[vi++] = bg.y;
                vertices[vi++] = bg.z;
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
                           .AddLayout(EDataType::Float, 2)
                           .AddLayout(EDataType::Float, 3)
                           .AddLayout(EDataType::Float, 3),
                       material);
        }
    }
    return model;
}

