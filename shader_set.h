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
    TTexture Sky;
    TTexture Shadow;
    glm::mat4 LightMatrix;
    TSceneShader *SceneShader;
    TParticlesShader *ParticlesShader;
    glm::vec3 Position;

public:
    TSceneShaderSet(TSceneShader *scene, TParticlesShader *particles, TTexture sky,
                    TTexture shadow, glm::mat4 lightMatrix, glm::vec3 position);
    void Particles(glm::mat4 model, glm::mat4 single, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TMaterial &mat, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) override;
    glm::vec3 GetPosition() override { return Position; }
};

class TShadowShaderSet: public IShaderSet {
private:
    TShadowShader *ShadowShader;
    glm::mat4 LightMatrix;
    glm::vec3 Position;

public:
    TShadowShaderSet(TShadowShader *shader, glm::mat4 lightMatrix, glm::vec3 position);
    void Particles(glm::mat4 model, glm::mat4 single, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TMaterial &mat, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) override;
    glm::vec3 GetPosition() override { return Position; }
};



