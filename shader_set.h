#pragma once
#include "model.h"
#include "shaders/scene.h"
#include "shaders/particles.h"
#include <glm/glm.hpp>

class IShaderSet {
public:
    virtual ~IShaderSet() = default;
    virtual void Particles(glm::mat4 model, glm::mat4 single, const TMesh &mesh) = 0;
    virtual void Scene(glm::mat4 model, bool opaque, float explosion, const TMaterial &mat, const TMesh &mesh) = 0;
    virtual void Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) = 0;
    virtual glm::vec3 GetPosition() = 0;
};

class TSceneShaderSet : public IShaderSet {
private:
    TTexture Sky;
    TSceneShader *SceneShader;
    TParticlesShader *ParticlesShader;
    glm::vec3 Position;

public:
    TSceneShaderSet(TSceneShader *scene, TParticlesShader *particles, TTexture sky, glm::vec3 position);
    void Particles(glm::mat4 model, glm::mat4 single, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TMaterial &mat, const TMesh &mesh) override;
    void Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) override;
    glm::vec3 GetPosition() override { return Position; }
};




