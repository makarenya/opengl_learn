#include "errors.h"
#include "scene.h"
#include "framebuffer.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

void TScene::Draw(mat4 project, mat4 view, vec3 position) {
    glClearColor(.05, .01, .07, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    TGlError::Assert("clear");

    DrawObjects(project, view, position);
    DrawLightCubes(project, view);

    DrawBorder(project, view, position);
    DrawSkybox(project, view);
    DrawOpaques(project, view, position);
}

void TScene::DrawSkybox(glm::mat4 project, glm::mat4 view) {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    TSkyboxSetup setup(SkyboxShader, project, view);
    Skybox.Use(setup);
    Sky.Draw();
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void TScene::SetupLights(TSceneSetup& setup, glm::vec3 position) {
    setup.DirectionalLight(vec3(1.0f, -6.0f, 3.0f),
                           vec3(0.25f), vec3(0.6f), vec3(0.4f));
    setup.SpotLight(vec3(position.x, 16.0, position.z),
                    vec3(0.01f), vec3(0.3f), vec3(0.0f),
                    0.02, 0.0002);

    for (auto &spot : Spots) {
        setup.SpotLight(spot.first,
                        0.01f * spot.second, 0.3f * spot.second, spot.second,
                        0.04, 0.0012);
    }
}

mat4 Place(vec3 position, vec3 axis, float angle, vec3 s) {
    return NConstMath::Translate(position) * NConstMath::RotateAxis(angle, axis) * NConstMath::Scale(s);
}

void TScene::DrawObjects(glm::mat4 project, glm::mat4 view, glm::vec3 position) {
    TSceneSetup setup(SceneShader, project, view, position);
    SetupLights(setup, position);
    setup.Model(scale(translate(one<mat4>(), vec3(0.0f, -100.0f, 0.0f)), vec3(200.0f)));
    Asphalt.Use(setup);
    GroundCube.Draw();

    setup.Model(Place(vec3(6, 7.0, 44.0), vec3(.2, .4, -.1), 30.0f, vec3(10.0f)));
    Container.Use(setup);
    SimpleCube.Draw();

    setup.Model(one<mat4>());
    Suit.Draw(setup, [this, &setup](const std::string &name, const TMesh &mesh) {
        if (name == "Visor") {
            SkyTex.Bind(setup.Skybox(0.8));
        } else {
            SkyTex.Bind(setup.Skybox(0.1));
        }
        mesh.Draw();
    });
}

void TScene::DrawOpaques(glm::mat4 project, glm::mat4 view, glm::vec3 position) {
    TSceneSetup setup(SceneShader, project, view, position);
    SetupLights(setup, position);

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

void TScene::DrawBorder(mat4 project, mat4 view, vec3) {
    {
        TSilhouetteSetup setup(SilhouetteShader, project, view);
        auto binder = FrameBuffer.Bind();
        setup.Model(one<mat4>());
        Suit.Draw(setup);
    }
    {
        TBorderSetup setup(BorderShader);
        setup.Color(vec4(0, 1, .5, .3));
        FrameBuffer.Draw(setup);
    }
}

void TScene::DrawLightCubes(mat4 project, mat4 view) {
    TLightSetup setup(LightShader, project, view);
    for (auto &spot : Spots) {
        setup.Model(scale(translate(one<mat4>(), spot.first), vec3(.5f))).Color(spot.second);
        SimpleCube.Draw();
    }
}

