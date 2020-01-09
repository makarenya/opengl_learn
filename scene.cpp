#include "errors.h"
#include "scene.h"
#include "framebuffer.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <random>

using namespace std;
using namespace glm;

void TScene::Draw(mat4 project, mat4 view, vec3 position) {
    ProjectionView = {project, view};
    SetupLights(position);

    glClearColor(.05, .01, .07, 1);
    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    DrawObjects(position);
    DrawLightCubes();

    DrawSkybox();
    DrawBorder();
    DrawOpaques(position);

    double meter = 15 / 1.8;
    double gravity = 9.8 * meter;
    std::random_device rd;
    std::mt19937_64 e2(rd());
    std::normal_distribution<> dist(0, .2 * meter);
    std::normal_distribution<> vdist(5 * meter, .5 * meter);
    if (std::isnan(LastParticleTime)) {
        LastParticleTime = glfwGetTime();
    } else {
        double now = glfwGetTime();
        auto interval = static_cast<float>(now - LastParticleTime);
        LastParticleTime = now;
        if (now - ExplosionCounter > 15) {
            ExplosionCounter = now;
        }
        ExplosionTime = now - ExplosionCounter;
        for (int i = 0; i < 30 && CurrentParticles < Particles.size(); ++i) {
            Particles[CurrentParticles++] = make_tuple(vec3(0, 0, 0),
                                                       vec3(std::round(dist(e2)),
                                                            std::round(vdist(e2)),
                                                            std::round(dist(e2))));
        }
        TArrayBufferMapper<vec3> mapper(Points.GetInstances());
        auto p = *mapper;
        for (int i = 0; i < CurrentParticles; ++i) {
            auto[pos, speed] = Particles[i];
            speed = vec3(speed.x, speed.y - gravity * interval, speed.z);
            if (speed.y < -2 * meter) {
                vec3 add = 3.0f * vec3(Currents - Maxims / 2) / vec3(Maxims);
                Particles[i] = make_tuple(vec3(0, std::round(dist(e2)), 0),
                                          vec3(std::round(dist(e2)), std::round(vdist(e2)), std::round(dist(e2)))
                                              + add);
                Currents = {Currents.x >= Maxims.x - 1 ? 0 : Currents.x + 1,
                            Currents.y >= Maxims.y - 1 ? 0 : Currents.y + 1,
                            Currents.z >= Maxims.z - 1 ? 0 : Currents.z + 1};
            } else {
                Particles[i] = make_tuple(pos + speed * interval, speed);
            }
            *p++ = pos;
            *p++ = speed;
        }
    }
    {
        auto setup = TParticlesSetup(&ParticlesShader)
            .SetViewPos(position)
            .SetModel(NConstMath::Translate(0, 10, 0))
            .SetSingle(NConstMath::Scale(.5))
            .SetSkyBox(SkyTex);
        Points.Draw();
    }
}

void TScene::DrawSkybox() {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    auto setup = TSkyBoxSetup(&SkyboxShader).SetSkyBox(SkyTex);
    Sky.Draw();
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void TScene::SetupLights(glm::vec3 position) {
    TLights setup{};
    setup.directional = {{1.0f, -6.0f, 3.0f},
                         vec3(.01), vec3(.4), vec3(.4)};
    setup.spots[0] = {{position.x, 16.0, position.z},
                      vec3(0.01f), vec3(0.3f), vec3(1.0f),
                      0.09, 0.0032};
    int k = 1;
    for (auto &spot : Spots) {
        setup.spots[k] = {spot.first,
                          0.01f * spot.second,
                          0.3f * spot.second,
                          spot.second,
                          0.02, 0.0009};
        k++;
    }
    setup.spotCount = k;
    LightSetup = setup;
}

mat4 Place(vec3 position, vec3 axis, float angle, vec3 s) {
    return NConstMath::Translate(position) * NConstMath::RotateAxis(angle, axis) * NConstMath::Scale(s);
}

void TScene::DrawObjects(glm::vec3 position) {
    auto setup = TSceneSetup(&SceneShader)
        .SetViewPos(position)
        .SetSkyBox(SkyTex)
        .SetCanDiscard(false);
    setup.SetModel(scale(translate(one<mat4>(), vec3(0.0f, -100.0f, 0.0f)), vec3(200.0f)));
    Asphalt.DrawWith(setup, GroundCube);

    setup.SetModel(Place(vec3(6, 7.0, 44.0), vec3(.2, .4, -.1), 30.0f, vec3(10.0f)));
    Container.DrawWith(setup, SimpleCube);

    setup.SetModel(NConstMath::Translate(0, 0, -15));
    if (ExplosionTime > 14) {
        setup.SetExplosion(std::sin((ExplosionTime - 14) * M_PI) * 20);
    } else {
        setup.SetExplosion(0);
    }
    Suit.Draw(setup);
}

void TScene::DrawOpaques(glm::vec3 position) {
    auto setup = TSceneSetup(&SceneShader).SetViewPos(position);
    setup.SetCanDiscard(true);
    std::map<float, OpaqueType> objs{};
    for (auto obj : OpaqueObjects) {
        objs.emplace(-glm::length(get<0>(obj) - position), obj);
    }
    for (auto obj : objs) {
        auto&[position, matrix, material, mesh] = obj.second;
        setup.SetModel(NConstMath::Translate(position) * matrix);
        material.DrawWith(setup, mesh);
    }
    setup.SetCanDiscard(false);
}

void TScene::DrawBorder() {
    {
        auto setup = TSilhouetteSetup(&SilhouetteShader).SetModel(NConstMath::Translate(0, 0, -15));
        auto binder = AliasedFrameBuffer.Bind();
        Suit.Draw(setup);
    }
    AliasedFrameBuffer.Copy(FrameBuffer);
    {
        TBorderSetup setup(&BorderShader);
        setup.SetColor(vec4(0, 1, .5, .3)).SetKernel(2.4, 1.2);
        FrameBuffer.Draw(setup);
    }
}

void TScene::DrawLightCubes() {
    TLightSetup setup(&LightShader);
    for (auto &spot : Spots) {
        setup.SetModel(scale(translate(one<mat4>(), spot.first), vec3(.5f))).SetColor(spot.second);
        SimpleCube.Draw();
    }
}

