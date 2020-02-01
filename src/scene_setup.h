#pragma once
#include <glm/glm.hpp>
#include <random>

struct TProjectionView {
    glm::mat4 projection;
    glm::mat4 view;
};

struct TDirectionalLight {
    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    TDirectionalLight() = default;
    TDirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
        : Ambient(ambient, 1.0)
          , Diffuse(diffuse, 1.0)
          , Specular(specular, 1.0) {
    }
};

struct TSpotLight {
    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec3 Specular;
    float Linear;
    float Quadratic;
    float tmp[3]{};
    TSpotLight() = default;
    TSpotLight(glm::vec3 ambient,
               glm::vec3 diffuse,
               glm::vec3 specular,
               float linear,
               float quadratic)
        : Ambient(ambient, 1.0)
          , Diffuse(diffuse, 1.0)
          , Specular(specular)
          , Linear(linear)
          , Quadratic(quadratic) {
    }
};

struct TProjectorLightPos {
    glm::vec4 Position;
    glm::vec4 Target;

    TProjectorLightPos() = default;
    TProjectorLightPos(glm::vec3 position,
                    glm::vec3 target)
        : Position(position, 1.0f)
          , Target(target, 1.0f) {
    }
};

struct TProjectorLight {
    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec3 Specular;

    float InnerCutoff;
    float OuterCutoff;
    float Linear;
    float Quadratic;
    float tmp;
    TProjectorLight() = default;
    TProjectorLight(glm::vec3 ambient,
                    glm::vec3 diffuse,
                    glm::vec3 specular,
                    float innerCutoff,
                    float outerCutoff,
                    float linear,
                    float quadratic)
        : Ambient(ambient, 1.0f)
          , Diffuse(diffuse, 1.0f)
          , Specular(specular)
          , InnerCutoff(std::cos(glm::radians(innerCutoff)))
          , OuterCutoff(std::cos(glm::radians(outerCutoff)))
          , Linear(linear)
          , Quadratic(quadratic)
          , tmp{} {
    }
};

struct TLights {
    TDirectionalLight directional;
    TSpotLight spots[4];
    TProjectorLight projector;
    uint32_t spotCount;
};

struct TLightsPos {
    glm::vec4 directional;
    glm::vec4 spots[4];
    TProjectorLightPos projector;
};

class TParticleInjector {
private:
    static constexpr double Meter = 15 / 1.8;
    static constexpr glm::ivec3 Maxims{53, 37, 47};
    glm::ivec3 Currents{};
    std::random_device Rd{};
    std::mt19937_64 E2{Rd()};
    std::normal_distribution<> Dist{0, .2 * Meter};
    std::normal_distribution<> VDist{5 * Meter, .5 * Meter};

public:
    std::pair<glm::vec3, glm::vec3> Inject() {
        glm::vec3 add = 3.0f * glm::vec3(Currents - Maxims / 2) / glm::vec3(Maxims);
        glm::vec3 dir{std::round(Dist(E2)), std::round(VDist(E2)), std::round(Dist(E2))};
        Currents = {Currents.x >= Maxims.x - 1 ? 0 : Currents.x + 1,
                    Currents.y >= Maxims.y - 1 ? 0 : Currents.y + 1,
                    Currents.z >= Maxims.z - 1 ? 0 : Currents.z + 1};
        return std::make_pair(glm::vec3(0, std::round(Dist(E2)), 0), dir + add);
    }
};
