#include "shader_set.h"

TSceneShaderSet::TSceneShaderSet(TSceneShader *scene,
                                 TParticlesShader *particles,
                                 TCubeTexture sky,
                                 TFlatTexture shadow,
                                 TCubeTexture spotShadow,
                                 TCubeTexture spotShadow2,
                                 glm::mat4 lightMatrix,
                                 glm::vec3 position,
                                 bool useMap)
    : SceneShader(scene)
      , ParticlesShader(particles)
      , Sky(std::move(sky))
      , Shadow(std::move(shadow))
      , SpotShadow(std::move(spotShadow))
      , SpotShadow2(std::move(spotShadow2))
      , LightMatrix(lightMatrix)
      , Position(position)
      , UseMap(useMap) {
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
        .SetShadow(Shadow)
        .SetSpotShadow(SpotShadow)
        .SetSpotShadow2(SpotShadow2)
        .SetLight(LightMatrix * model)
        .SetExplosion(explosion)
        .SetUseMap(UseMap);
    mat.DrawWith(setup, mesh);
}

void TSceneShaderSet::Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) {
    auto setup = TSceneSetup(SceneShader)
        .SetViewPos(Position)
        .SetModel(model)
        .SetOpaque(opaque)
        .SetShadow(Shadow)
        .SetSpotShadow(SpotShadow)
        .SetSpotShadow2(SpotShadow2)
        .SetLight(LightMatrix * model)
        .SetExplosion(explosion)
        .SetUseMap(UseMap);
    obj.Draw(setup);
}

TShadowShaderSet::TShadowShaderSet(TShadowShader *shader, glm::mat4 lightMatrix, glm::vec3 position)
    : ShadowShader(shader)
      , LightMatrices({lightMatrix})
      , Position(position)
      , Direct(true) {
}

TShadowShaderSet::TShadowShaderSet(TShadowShader *shader,
                                   const std::array<glm::mat4, 6> &lightMatrices,
                                   glm::vec3 lightPos,
                                   glm::vec3 position)
    : ShadowShader(shader)
      , LightMatrices(lightMatrices)
      , Position(position)
      , LightPos(lightPos)
      , Direct(false) {
}

void TShadowShaderSet::Particles(glm::mat4, glm::mat4, const TMesh &mesh) {
}

void TShadowShaderSet::Scene(glm::mat4 model, bool opacity, float explosion, const TMaterial &mat, const TMesh &mesh) {
    if (explosion > 0) return;
    auto setup = TShadowSetup(ShadowShader)
        .SetLightMatrices(LightMatrices)
        .SetDirect(Direct)
        .SetModel(model)
        .SetLightPos(LightPos)
        .SetOpacity(opacity);
    mat.DrawWith(setup, mesh);
}

void TShadowShaderSet::Scene(glm::mat4 model, bool opacity, float explosion, const TModel &obj) {
    if (explosion > 0) return;
    auto setup = TShadowSetup(ShadowShader)
        .SetLightMatrices(LightMatrices)
        .SetDirect(Direct)
        .SetModel(model)
        .SetLightPos(LightPos)
        .SetOpacity(opacity);
    obj.Draw(setup);
}
