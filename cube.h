#pragma once
#include "common.h"
#include "const_math.h"

class TGeomBuilder {
public:
    constexpr TGeomBuilder() = default;
    BUILDER_PROPERTY(float, Size){.5};
    BUILDER_PROPERTY(bool, Backward){false};
    BUILDER_PROPERTY3(float, float, float, Position){0, 0, 0};
    BUILDER_PROPERTY2(float, float, TextureMul){1, 1};
    BUILDER_PROPERTY2(float, float, TextureOffset){0, 0};
};

namespace Impl {
    constexpr std::array<std::tuple<glm::vec2, glm::vec2>, 6> QuadPoints = {
        std::tuple{glm::vec2{1.0f, 1.0f}, glm::vec2{1, 0}}, // upper right
        std::tuple{glm::vec2{-1.0f, 1.0f}, glm::vec2{0, 0}}, // upper left
        std::tuple{glm::vec2{1.0f, -1.0f}, glm::vec2{1, 1}}, // lower right

        std::tuple{glm::vec2{1.0f, -1.0f}, glm::vec2{1, 1}}, // lower right
        std::tuple{glm::vec2{-1.0f, 1.0f}, glm::vec2{0, 0}}, // upper left
        std::tuple{glm::vec2{-1.0f, -1.0f}, glm::vec2{0, 1}}, // lower left
    };

    constexpr std::array<std::tuple<glm::vec2, glm::vec2>, 6> BackQuadPoints = {
        std::tuple{glm::vec2{1.0f, 1.0f}, glm::vec2{1, 0}}, // upper right
        std::tuple{glm::vec2{1.0f, -1.0f}, glm::vec2{1, 1}}, // lower right
        std::tuple{glm::vec2{-1.0f, 1.0f}, glm::vec2{0, 0}}, // upper left

        std::tuple{glm::vec2{1.0f, -1.0f}, glm::vec2{1, 1}}, // lower right
        std::tuple{glm::vec2{-1.0f, -1.0f}, glm::vec2{0, 1}}, // lower left
        std::tuple{glm::vec2{-1.0f, 1.0f}, glm::vec2{0, 0}}, // upper left
    };

    constexpr glm::vec3 Tangent{NConstMath::Normalize(std::get<0>(QuadPoints[0]) - std::get<0>(QuadPoints[1])), 0};
    constexpr glm::vec3 BiTangent{NConstMath::Normalize(std::get<0>(QuadPoints[5]) - std::get<0>(QuadPoints[1])), 0};
    constexpr glm::vec3 Normal { 0, 0, 1 };
}


template<bool Normals = true, bool Texture = true, bool Tangents = false>
constexpr auto Quad(TGeomBuilder builder = {}) {
    using namespace std;
    const glm::vec3 pos{get<0>(builder.Position_), get<1>(builder.Position_), get<2>(builder.Position_)};
    constexpr int stride = (Normals ? 3 : 0) + (Texture ? 2 : 0) + (Tangents ? 6 : 0) + 3 * 1.0f;
    std::array<GLfloat, 6 * stride> vertices{};
    size_t k = 0;
    for (auto point : Impl::QuadPoints) {
        auto r = glm::vec3(std::get<0>(point), 0.0f) * builder.Size_ + pos;
        vertices[k++] = r.x;
        vertices[k++] = r.y;
        vertices[k++] = r.z;
        if (Normals) {
            vertices[k++] = Impl::Normal.x;
            vertices[k++] = Impl::Normal.y;
            vertices[k++] = Impl::Normal.z;
        }
        if (Texture) {
            auto t = std::get<1>(point);
            vertices[k++] = t.x * get<0>(builder.TextureMul_) + get<0>(builder.TextureOffset_);
            vertices[k++] = t.y * get<1>(builder.TextureMul_) + get<1>(builder.TextureOffset_);
        }
        if (Tangents) {
            vertices[k++] = Impl::Tangent.x;
            vertices[k++] = Impl::Tangent.y;
            vertices[k++] = Impl::Tangent.z;
            vertices[k++] = Impl::BiTangent.x;
            vertices[k++] = Impl::BiTangent.y;
            vertices[k++] = Impl::BiTangent.z;
        }
    }
    return vertices;
}

template<bool Normals = true, bool Texture = true, bool Tangents = false>
constexpr auto DoubleQuad(TGeomBuilder builder = {}) {
    using namespace std;
    using namespace glm;
    const glm::vec3 pos{get<0>(builder.Position_), get<1>(builder.Position_), get<2>(builder.Position_)};
    constexpr int stride = (Normals ? 3 : 0) + (Texture ? 2 : 0) + (Tangents ? 6 : 0) + 3 * 1.0f;
    std::array<GLfloat, 12 * stride> vertices{};
    size_t k = 0;
    for (auto pset : {tuple(Impl::QuadPoints, vec3(0, 0, 1)), tuple(Impl::BackQuadPoints, vec3(0, 0, -1))}) {
        for (auto point : get<0>(pset)) {
            auto r = glm::vec3(std::get<0>(point), 0.0f) * builder.Size_ + pos;
            vertices[k++] = r.x;
            vertices[k++] = r.y;
            vertices[k++] = r.z;
            if (Normals) {
                auto n = get<1>(pset);
                vertices[k++] = n.x;
                vertices[k++] = n.y;
                vertices[k++] = n.z;
            }
            if (Texture) {
                auto t = std::get<1>(point);
                vertices[k++] = t.x * get<0>(builder.TextureMul_) + get<0>(builder.TextureOffset_);
                vertices[k++] = t.y * get<1>(builder.TextureMul_) + get<1>(builder.TextureOffset_);
            }
            if (Tangents) {
                vertices[k++] = Impl::Tangent.x;
                vertices[k++] = Impl::Tangent.y;
                vertices[k++] = Impl::Tangent.z;
                vertices[k++] = Impl::BiTangent.x;
                vertices[k++] = Impl::BiTangent.y;
                vertices[k++] = Impl::BiTangent.z;
            }
        }
    }
    return vertices;
}

template<bool Normals = true, bool Texture = true, bool Tangents = false>
constexpr auto Cube(TGeomBuilder builder = {}) {
    using namespace std;
    const glm::vec3 pos{get<0>(builder.Position_), get<1>(builder.Position_), get<2>(builder.Position_)};
    constexpr int stride = (Normals ? 3 : 0) + (Texture ? 2 : 0) + (Tangents ? 6 : 0) + 3 * 1.0f;
    std::array<glm::mat3, 6> rotations = {
        NConstMath::RotateX3(0),
        NConstMath::RotateX3(M_PI_2),
        NConstMath::RotateX3(M_PI),
        NConstMath::RotateX3(-M_PI_2),
        NConstMath::RotateY3(M_PI_2),
        NConstMath::RotateY3(-M_PI_2)
    };
    std::array<GLfloat, stride * rotations.size() * 6> vertices{};
    size_t k = 0;
    for (auto rotation : rotations) {
        for (auto point : builder.Backward_ ? Impl::BackQuadPoints : Impl::QuadPoints) {
            auto r = rotation * glm::vec3(std::get<0>(point), 1.0f) * builder.Size_ + pos;
            vertices[k++] = r.x;
            vertices[k++] = r.y;
            vertices[k++] = r.z;
            if (Normals) {
                auto n = rotation * Impl::Normal;
                vertices[k++] = n.x;
                vertices[k++] = n.y;
                vertices[k++] = n.z;
            }
            if (Texture) {
                auto t = std::get<1>(point);
                vertices[k++] = t.x * get<0>(builder.TextureMul_) + get<0>(builder.TextureOffset_);
                vertices[k++] = t.y * get<1>(builder.TextureMul_) + get<1>(builder.TextureOffset_);
            }
            if (Tangents) {
                auto t = rotation * Impl::Tangent;
                auto b = rotation * Impl::BiTangent;
                vertices[k++] = t.x;
                vertices[k++] = t.y;
                vertices[k++] = t.z;
                vertices[k++] = b.x;
                vertices[k++] = b.y;
                vertices[k++] = b.z;
            }
        }
    }
    return vertices;
}
