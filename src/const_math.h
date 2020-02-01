#include <cmath>
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
        while (x >= 2 * M_PI) {
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
        return SinInt(x - 2 * M_PI);
    }

    constexpr double Cos(double x) {
        return Sin(x + M_PI_2);
    }

    constexpr size_t BitLength(size_t x) {
        if (x == 0) return 0;
        else {
            return BitLength(x >> 1U) + 1;
        }
    }

    constexpr double Sqrt(double x) {
        if (x == 0) return 0;
        if (x < 0) return static_cast<double>(NAN);
        double a = x >= 1 ? 1ULL << ((BitLength(x) - 1) / 2) : 1.0 / (1ULL << ((BitLength(1 / x) - 1) / 2));
        for (int i = 0; i < 20; i++) {
            a = (a + x / a) / 2.0;
        }
        return a;
    }

    constexpr double AtanInt(double x) {
        const double SQRT3 = 1.73205080756887729352744634150587236;
        if (x > M_PI / 12) {
            return M_PI / 6 + AtanInt((x * SQRT3 - 1) / (x + SQRT3));
        } else {
            return x * (0.55913709 / (1.4087812 + x * x) + 0.60310579 - 0.05160454 * x * x);
        }
    }

    constexpr double Atan(double x) {
        if (x < 0) {
            return -AtanInt(-x);
        } else {
            return AtanInt(x);
        }
    }

    constexpr double Asin(double x) {
        if (x > 1 || x < -1) return NAN;
        if (x == 1) return M_PI_2;
        if (x == -1) return -M_PI_2;
        return Atan(x / Sqrt((1 - x * x)));
    }

    constexpr double Acos(double x) {
        return M_PI_2 - Asin(x);
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

    constexpr glm::mat3 RotateZ3(double angle) {
        return glm::mat3{
            Cos(angle), Sin(angle), 0,
            -Sin(angle), Cos(angle), 0,
            0, 0, 1
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
                d.x * d.z / c, -d.y * d.z / c, c, 0,
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
