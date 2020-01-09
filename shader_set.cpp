#include "shader_set.h"

TSceneShaderSet::TSceneShaderSet(TSceneShader *scene, TParticlesShader *particles, TTexture sky, glm::vec3 position)
    : SceneShader(scene)
      , ParticlesShader(particles)
      , Sky(std::move(sky))
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
        .SetCanDiscard(opaque)
        .SetSkyBox(Sky)
        .SetExplosion(explosion);
    mat.DrawWith(setup, mesh);
}

void TSceneShaderSet::Scene(glm::mat4 model, bool opaque, float explosion, const TModel &obj) {
    auto setup = TSceneSetup(SceneShader)
        .SetViewPos(Position)
        .SetModel(model)
        .SetCanDiscard(opaque)
        .SetSkyBox(Sky)
        .SetExplosion(explosion);
    obj.Draw(setup);
}
