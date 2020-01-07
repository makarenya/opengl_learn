#include "errors.h"
#include "scene.h"
#include "framebuffer.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <random>
#include <iostream>

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
        TVertexBufferMapper mapper(Points, true);
        auto p = mapper.Ptr<vec3>();
        for (int i = 0; i < CurrentParticles; ++i) {
            auto[pos, speed] = Particles[i];
            speed = vec3(speed.x, speed.y - gravity * interval, speed.z);
            if (speed.y < -3 * meter) {
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
    TParticlesSetup setup(ParticlesShader);
    setup.Skybox(SkyTex);
    setup.ViewPos(position);
    setup.Model(one<mat4>());
    Points.Draw();
}

void TScene::DrawSkybox() {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    TSkyboxSetup setup(SkyboxShader);
    Skybox.Use(setup);
    Sky.Draw();
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void TScene::SetupLights(glm::vec3 position) {
    TLights setup{};
    setup.directional = {{1.0f, -6.0f, 3.0f},
                         vec3(.25), vec3(.6), vec3(.4)};
    setup.spots[0] = {{position.x, 16.0, position.z},
                      vec3(0.01f), vec3(0.3f), vec3(1.0f),
                      0.02, 0.0002};
    int k = 1;
    for (auto &spot : Spots) {
        setup.spots[k] = {spot.first,
                          0.01f * spot.second,
                          0.3f * spot.second,
                          spot.second,
                          0.04, 0.0012};
        k++;
    }
    setup.spotCount = k;
    LightSetup = setup;
}

mat4 Place(vec3 position, vec3 axis, float angle, vec3 s) {
    return NConstMath::Translate(position) * NConstMath::RotateAxis(angle, axis) * NConstMath::Scale(s);
}

void TScene::DrawObjects(glm::vec3 position) {
    {
        TSceneSetup setup(SceneShader, position);
        setup.Model(scale(translate(one<mat4>(), vec3(0.0f, -100.0f, 0.0f)), vec3(200.0f)));
        Asphalt.Use(setup);
        GroundCube.Draw();

        setup.Model(Place(vec3(6, 7.0, 44.0), vec3(.2, .4, -.1), 30.0f, vec3(10.0f)));
        Container.Use(setup);
        SkyTex.Bind(setup.Skybox());
        setup.Reflection(0.1);
        SimpleCube.Draw();
        setup.Model(NConstMath::Translate(0, 0, -15));
        Suit.Draw(setup, [this, &setup](const std::string &name, const TMesh &mesh) {
            SkyTex.Bind(setup.Skybox());
            if (ExplosionTime > 14) {
                setup.Explosion(std::sin((ExplosionTime - 14) * M_PI_4) * 20);
            } else {
                setup.Explosion(0);
            }
            if (name == "Visor") {
                setup.Reflection(0.5);
            } else {
                setup.Reflection(0.2);
            }
            mesh.Draw();
        });
        setup.Explosion(0);
        setup.NoReflectRefract();
    }
}

void TScene::DrawOpaques(glm::vec3 position) {
    TSceneSetup setup(SceneShader, position);
    setup.CanDiscard(true);
    std::map<float, OpaqueType> objs{};
    for (auto obj : OpaqueObjects) {
        objs.emplace(-glm::length(get<0>(obj) - position), obj);
    }
    for (auto obj : objs) {
        auto&[position, matrix, material, mesh] = obj.second;
        setup.Model(NConstMath::Translate(position) * matrix);
        material.Use(setup);
        mesh.Draw();
    }
    setup.CanDiscard(false);
}

void TScene::DrawBorder() {
    {
        TSilhouetteSetup setup(SilhouetteShader);
        auto binder = FrameBuffer.Bind();
        setup.Model(NConstMath::Translate(0, 0, -15));
        Suit.Draw(setup);
    }
    {
        TBorderSetup setup(BorderShader);
        setup.Color(vec4(0, 1, .5, .3));
        FrameBuffer.Draw(setup);
    }
}

void TScene::DrawLightCubes() {
    TLightSetup setup(LightShader);
    for (auto &spot : Spots) {
        setup.Model(scale(translate(one<mat4>(), spot.first), vec3(.5f))).Color(spot.second);
        SimpleCube.Draw();
    }
}

