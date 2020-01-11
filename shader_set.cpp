#include "shader_set.h"

TSceneShaderSet::TSceneShaderSet(TSceneShader *scene,
                                 TParticlesShader *particles,
                                 TTexture sky,
                                 TTexture shadow,
                                 glm::mat4 lightMatrix,
                                 glm::vec3 position)
    : SceneShader(scene)
      , ParticlesShader(particles)
      , Sky(std::move(sky))
      , Shadow(std::move(shadow))
      , LightMatrix(lightMatrix)
      , Position(position) {
}

void TSceneShaderSet::Particles(glm::mat4 model, glm::mat4 single, const TMesh &mesh) {
    auto setup = TParticlesSetup(ParticlesShader)
        .SetViewPos(Position)
        .SetModel(model)
        .SetSingle(single)
        .SetSkyBox(Sky);
    mesh.Draw();
}

void TSceneShaderSet::Scene(glm::mat4 model, bool opaque, float explosion, const TMaterial &mat, const TMesh &mesh) {
    auto setup = TSceneSetup(SceneShader)
        .SetViewPos(Position)
        .SetModel(model)
        .SetOpaque(opaque)
        .SetSkyBox(Sky)
        .SetShadow(Shadow)
        .SetLightMatrix(LightMatrix)
        .SetExplosion(explosion);
    mat.DrawWith(setup, mesh);
}

void TSceneShaderSet::Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) {
    auto setup = TSceneSetup(SceneShader)
        .SetViewPos(Position)
        .SetModel(model)
        .SetOpaque(opaque)
        .SetSkyBox(Sky)
        .SetShadow(Shadow)
        .SetLightMatrix(LightMatrix)
        .SetExplosion(explosion);
    obj.Draw(setup);
}

TShadowShaderSet::TShadowShaderSet(TShadowShader *shader, glm::mat4 lightMatrix, glm::vec3 position)
    : ShadowShader(shader)
      , LightMatrix(lightMatrix)
      , Position(position) {
}

void TShadowShaderSet::Particles(glm::mat4, glm::mat4, const TMesh &mesh) {
}

void TShadowShaderSet::Scene(glm::mat4 model, bool opacity, float explosion, const TMaterial &mat, const TMesh &mesh) {
    if (explosion > 0) return;
    auto setup = TShadowSetup(ShadowShader)
        .SetLightMatrix(LightMatrix)
        .SetModel(model)
        .SetOpacity(opacity);
    mat.DrawWith(setup, mesh);
}

void TShadowShaderSet::Scene(glm::mat4 model, bool opacity, float explosion, const TModel &obj) {
    if (explosion > 0) return;
    auto setup = TShadowSetup(ShadowShader)
        .SetLightMatrix(LightMatrix)
        .SetModel(model)
        .SetOpacity(opacity);
    obj.Draw(setup);
}
