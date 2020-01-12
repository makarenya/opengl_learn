#pragma once
#include "model.h"
#include "shaders/scene.h"
#include "shaders/particles.h"
#include "shaders/shadow.h"
#include "shaders/depth.h"
#include <glm/glm.hpp>

class IShaderSet {
public:
    virtual ~IShaderSet() = default;
    virtual void Particles(glm::mat4 model, glm::mat4 single, const TMesh &mesh) = 0;
    virtual void Scene(glm::mat4 model, bool opaque, float explosion, const TMaterial &mat, const TMesh &mesh) = 0;
    virtual void Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) = 0;
    virtual glm::vec3 GetPosition() = 0;
};

class TSceneShaderSet: public IShaderSet {
private:
    TCubeTexture Sky;
    TFlatTexture Shadow;
    TCubeTexture SpotShadow;
    TCubeTexture SpotShadow2;
    glm::mat4 LightMatrix;
    TSceneShader *SceneShader;
    TParticlesShader *ParticlesShader;
    glm::vec3 Position;

public:
    TSceneShaderSet(TSceneShader *scene, TParticlesShader *particles, TCubeTexture sky,
                    TFlatTexture shadow, TCubeTexture spotShadow, TCubeTexture spotShadow2,
                    glm::mat4 lightMatrix, glm::vec3 position);
    void Particles(glm::mat4 model, glm::mat4 single, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TMaterial &mat, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) override;
    glm::vec3 GetPosition() override { return Position; }
};

class TShadowShaderSet: public IShaderSet {
private:
    TShadowShader *ShadowShader;
    std::array<glm::mat4, 6> LightMatrices;
    glm::vec3 LightPos;
    glm::vec3 Position;
    bool Direct;

public:
    TShadowShaderSet(TShadowShader *shader, glm::mat4 lightMatrix, glm::vec3 position);
    TShadowShaderSet(TShadowShader *shader, const std::array<glm::mat4, 6> &lightMatrices, glm::vec3 lightPos, glm::vec3 position);
    void Particles(glm::mat4 model, glm::mat4 single, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TMaterial &mat, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) override;
    glm::vec3 GetPosition() override { return Position; }
};



