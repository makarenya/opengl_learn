#pragma once
#include "common.h"

namespace NConstMath {
    constexpr double Fact(int n) {
        if (n == 0) return 1;
        return n * Fact(n - 1);
    }

    constexpr double Pow(double x, int n) {
        if (n == 0) return 1;
        return x * Pow(x, n - 1);
    }

    constexpr double SinInt(double x) {
        double a = 0;
        for (int i = 0; i < 10; i++) {
            double val = Pow(x, i * 2 + 1) / Fact(i * 2 + 1);
            a += (i % 2 == 0) ? val : -val;
        }
        return a;
    }

    constexpr double CosInt(double x) {
        double a = 0;
        for (int i = 0; i < 10; i++) {
            double val = Pow(x, i * 2) / Fact(i * 2);
            a += (i % 2 == 0) ? val : -val;
        }
        return a;
    }

    constexpr double Norm(double x) {
        while (x >= M_2_PI) {
            x -= 2 * M_PI;
        }
        while (x < 0) {
            x += 2 * M_PI;
        }
        return x;
    }

    constexpr double Sin(double x) {
        x = Norm(x);
        if (x < M_PI_4) {
            return SinInt(x);
        }
        if (x < 3 * M_PI / 4) {
            return CosInt(x - M_PI_2);
        }
        if (x < 5 * M_PI / 4) {
            return -SinInt(x - M_PI);
        }
        if (x < 7 * M_PI / 4) {
            return -CosInt(x - 3 * M_PI / 2);
        }
        return SinInt(x - M_2_PI);
    }

    constexpr double Cos(double x) {
        return Sin(x + M_PI_2);
    }

    constexpr double Sqrt(double x) {
        if (x == 0) return 0;
        double a = x;
        for (int i = 0; i < 20; i++) {
            a = (a + x / a) / 2.0;
        }
        return a;
    }

    constexpr glm::vec2 Normalize(glm::vec2 v) {
        double length = Sqrt(v.x * v.x + v.y * v.y);
        return glm::vec2(v.x / length, v.y / length);

    }
    constexpr glm::vec3 Normalize(glm::vec3 v) {
        double length = Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return glm::vec3(v.x / length, v.y / length, v.z / length);
    }

    constexpr glm::mat3 RotateX3(double angle) {
        return glm::mat3{
            1, 0, 0,
            0, Cos(angle), Sin(angle),
            0, -Sin(angle), Cos(angle)
        };
    };

    constexpr glm::mat3 RotateY3(double angle) {
        return glm::mat3{
            Cos(angle), 0, -Sin(angle),
            0, 1, 0,
            Sin(angle), 0, Cos(angle)
        };
    };

    constexpr glm::mat4 RotateX4(double angle) {
        return glm::mat4{
            1, 0, 0, 0,
            0, Cos(angle), Sin(angle), 0,
            0, -Sin(angle), Cos(angle), 0,
            0, 0, 0, 1
        };
    };

    constexpr glm::mat4 RotateY4(double angle) {
        return glm::mat4{
            Cos(angle), 0, -Sin(angle), 0,
            0, 1, 0, 0,
            Sin(angle), 0, Cos(angle), 0,
            0, 0, 0, 1
        };
    };

    constexpr glm::mat4 RotateZ4(double angle) {
        return glm::mat4{
            Cos(angle), Sin(angle), 0, 0,
            -Sin(angle), Cos(angle), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    };

    constexpr glm::mat4 RotateTo(glm::vec3 destination) {
        glm::vec3 d = Normalize(destination);
        if (d.z == 1 || d.z == -1) {
            return glm::mat4{
                1, 0, 0, 0,
                0, 0, d.z, 0,
                0, -d.z, 0, 0,
                0, 0, 0, 1
            };
        } else {
            float c = Sqrt(1 - d.z * d.z);
            return glm::mat4{
                d.y / c, d.x / c, 0, 0,
                -d.x, d.y, d.z, 0,
                d.x * d.z / c, - d.y * d.z / c, c, 0,
                0, 0, 0, 1
            };
        }
    }

    constexpr glm::mat4 Translate(double x, double y, double z) {
        return glm::mat4{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
        };
    };

    constexpr glm::mat4 Translate(glm::vec3 where) {
        return glm::mat4{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            where.x, where.y, where.z, 1
        };
    };

    constexpr glm::mat4 Scale(double scale) {
        return glm::mat4{
            scale, 0, 0, 0,
            0, scale, 0, 0,
            0, 0, scale, 0,
            0, 0, 0, 1
        };
    };

    constexpr glm::mat4 Scale(glm::vec3 scale) {
        return glm::mat4{
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1
        };
    };

    constexpr glm::mat4 RotateAxis(double angle, glm::vec3 v) {
        auto c = static_cast<float>(Cos(angle));
        auto s = static_cast<float>(Sin(angle));

        glm::vec3 axis(Normalize(v));
        glm::vec3 temp((1.0f - c) * axis);

        return glm::mat4{
            c + temp[0] * axis[0], temp[1] * axis[0] + s * axis[2], temp[2] * axis[0] - s * axis[1], 0,
            temp[0] * axis[1] - s * axis[2], c + temp[1] * axis[1], temp[2] * axis[1] + s * axis[0], 0,
            temp[0] * axis[2] + s * axis[1], temp[1] * axis[2] - s * axis[0], c + temp[2] * axis[2], 0,
            0, 0, 0, 1
        };
    }

}

constexpr int cube_stride = 8;

class TGeomBuilder {
public:
    constexpr TGeomBuilder() = default;
    BUILDER_PROPERTY(float, Size){.5};
    BUILDER_PROPERTY(bool, Backward){false};
    BUILDER_PROPERTY3(float, float, float, Position){0, 0, 0};
    BUILDER_PROPERTY2(float, float, TextureMul){1, 1};
    BUILDER_PROPERTY2(float, float, TextureOffset){0, 0};
};

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

template<bool Normals = true, bool Texture = true>
constexpr auto Quad(TGeomBuilder builder = {}) {
    using namespace std;
    const glm::vec3 pos{get<0>(builder.Position_), get<1>(builder.Position_), get<2>(builder.Position_)};
    constexpr int stride = (Normals ? 3 : 0) + (Texture ? 2 : 0) + 3 * 1.0f;
    std::array<GLfloat, 6 * stride> vertices{};
    size_t k = 0;
    const glm::vec3 n{0, 0, 1};
    for (auto point : QuadPoints) {
        auto r = glm::vec3(std::get<0>(point), 0.0f) * builder.Size_ + pos;
        vertices[k++] = r.x;
        vertices[k++] = r.y;
        vertices[k++] = r.z;
        if (Normals) {
            vertices[k++] = n.x;
            vertices[k++] = n.y;
            vertices[k++] = n.z;
        }
        if (Texture) {
            auto t = std::get<1>(point);
            vertices[k++] = t.x * get<0>(builder.TextureMul_) + get<0>(builder.TextureOffset_);
            vertices[k++] = t.y * get<1>(builder.TextureMul_) + get<1>(builder.TextureOffset_);
        }
    }
    return vertices;
}

template<bool Normals = true, bool Texture = true>
constexpr auto DoubleQuad(TGeomBuilder builder = {}) {
    using namespace std;
    using namespace glm;
    const glm::vec3 pos{get<0>(builder.Position_), get<1>(builder.Position_), get<2>(builder.Position_)};
    constexpr int stride = (Normals ? 3 : 0) + (Texture ? 2 : 0) + 3 * 1.0f;
    std::array<GLfloat, 12 * stride> vertices{};
    size_t k = 0;
    for (auto pset : {tuple(QuadPoints, vec3(0, 0, 1)), tuple(BackQuadPoints, vec3(0, 0, -1))}) {
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
        }
    }
    return vertices;
}

template<bool Normals = true, bool Texture = true>
constexpr std::array<GLfloat, 6 * 6 * cube_stride> Cube(TGeomBuilder builder = {}) {
    using namespace std;
    const glm::vec3 pos{get<0>(builder.Position_), get<1>(builder.Position_), get<2>(builder.Position_)};
    std::array<glm::mat3, 6> rotations = {
        NConstMath::RotateX3(0),
        NConstMath::RotateX3(M_PI_2),
        NConstMath::RotateX3(M_PI),
        NConstMath::RotateX3(-M_PI_2),
        NConstMath::RotateY3(M_PI_2),
        NConstMath::RotateY3(-M_PI_2)
    };
    std::array<GLfloat, 6 * rotations.size() * cube_stride> vertices{};
    size_t k = 0;
    glm::vec3 norm{0, 0, 1};
    for (auto rotation : rotations) {
        for (auto point : builder.Backward_ ? BackQuadPoints : QuadPoints) {
            auto r = rotation * glm::vec3(std::get<0>(point), 1.0f) * builder.Size_ + pos;
            vertices[k++] = r.x;
            vertices[k++] = r.y;
            vertices[k++] = r.z;
            if (Normals) {
                auto n = rotation * norm;
                vertices[k++] = n.x;
                vertices[k++] = n.y;
                vertices[k++] = n.z;
            }
            if (Texture) {
                auto t = std::get<1>(point);
                vertices[k++] = t.x * get<0>(builder.TextureMul_) + get<0>(builder.TextureOffset_);
                vertices[k++] = t.y * get<1>(builder.TextureMul_) + get<1>(builder.TextureOffset_);
            }
        }
    }
    return vertices;
}
