#include "errors.h"
#include "scene.h"
#include "framebuffer.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

using namespace std;
using namespace glm;

void TScene::Draw(mat4 project, mat4 view, vec3 position, float interval, bool useMap) {
    ExplosionTime = ExplosionTime >= 15 ? 0 : ExplosionTime + interval;
    SetupLights(position, interval);
    UpdateFountain(interval);

    mat4 lightMatrix = ortho(-70.0f, 70.0f, -70.0f, 70.0f, 0.01f, 150.0f) *
        lookAt(-normalize(Directional) * 60.0f, vec3(0, 0, 0), vec3(0, 1, 0));
    glm::mat4 proj = perspective(glm::radians(90.0f), 1.0f, 0.02f, 100.0f);

    glCullFace(GL_FRONT);
    {
        TFrameBufferBinder binder(GlobalLightShadow);
        DrawScene(TShadowShaderSet(&ShadowShader, lightMatrix, position));
    }
    {
        std::array<glm::mat4, 6> spotMatrices;
        spotMatrices[0] = proj * lookAt(Spots[0].first, Spots[0].first + vec3(1, 0, 0), vec3(0, -1, 0));
        spotMatrices[1] = proj * lookAt(Spots[0].first, Spots[0].first + vec3(-1, 0, 0), vec3(0, -1, 0));
        spotMatrices[2] = proj * lookAt(Spots[0].first, Spots[0].first + vec3(0, 1, 0), vec3(0, 0, 1));
        spotMatrices[3] = proj * lookAt(Spots[0].first, Spots[0].first + vec3(0, -1, 0), vec3(0, 0, -1));
        spotMatrices[4] = proj * lookAt(Spots[0].first, Spots[0].first + vec3(0, 0, 1), vec3(0, -1, 0));
        spotMatrices[5] = proj * lookAt(Spots[0].first, Spots[0].first + vec3(0, 0, -1), vec3(0, -1, 0));
        TFrameBufferBinder binder(SpotLightShadow);
        DrawScene(TShadowShaderSet(&ShadowShader, spotMatrices, Spots[0].first, position));
    }
    {
        std::array<glm::mat4, 6> spotMatrices;
        spotMatrices[0] = proj * lookAt(Spots[1].first, Spots[1].first + vec3(1, 0, 0), vec3(0, -1, 0));
        spotMatrices[1] = proj * lookAt(Spots[1].first, Spots[1].first + vec3(-1, 0, 0), vec3(0, -1, 0));
        spotMatrices[2] = proj * lookAt(Spots[1].first, Spots[1].first + vec3(0, 1, 0), vec3(0, 0, 1));
        spotMatrices[3] = proj * lookAt(Spots[1].first, Spots[1].first + vec3(0, -1, 0), vec3(0, 0, -1));
        spotMatrices[4] = proj * lookAt(Spots[1].first, Spots[1].first + vec3(0, 0, 1), vec3(0, -1, 0));
        spotMatrices[5] = proj * lookAt(Spots[1].first, Spots[1].first + vec3(0, 0, -1), vec3(0, -1, 0));
        TFrameBufferBinder binder(SpotLightShadow2);
        DrawScene(TShadowShaderSet(&ShadowShader, spotMatrices, Spots[1].first, position));
    }
    glCullFace(GL_BACK);
    {
        TFrameBufferBinder binder(AliasedFrameBuffer);
        ProjectionView = {project, view};
        DrawSkybox();
        DrawLightCubes();
        DrawScene(TSceneShaderSet{&SceneShader, &ParticlesShader, SkyTex,
                                  std::get<TFlatTexture>(GlobalLightShadow.GetDepth()),
                                  std::get<TCubeTexture>(SpotLightShadow.GetDepth()),
                                  std::get<TCubeTexture>(SpotLightShadow2.GetDepth()),
                                  lightMatrix, position, useMap});
    }
    AliasedFrameBuffer.CopyTo(FrameBuffer);
    {
        auto setup = THdrSetup(&HdrShader)
            .SetScreen(std::get<TFlatTexture>(FrameBuffer.GetScreen()))
            .SetDepth(std::get<TFlatTexture>(FrameBuffer.GetDepth()));
        glDepthFunc(GL_LEQUAL);
        ScreenQuad.Draw();
        glDepthFunc(GL_LESS);
    }
    DrawBorder();
}

void TScene::SetupLights(glm::vec3 position, float interval) {
    SpotAngle += interval;
    float radius = 5;
    Spots[0] = std::make_pair(glm::vec3{-4.0f + sin(SpotAngle) * radius, 13.0f, -6.0f + cos(SpotAngle) * radius},
                               get<1>(Spots[0]));
    TLights lights{};
    TLightsPos pos{};
    pos.directional = vec4(Directional, 1.0);
    lights.directional = {vec3(.005), vec3(.02), vec3(.2)};

    lights.spots[0] = {vec3(0.01f), vec3(0.3f), vec3(0.0f), 0.00, 0.03};
    pos.spots[0] = {position.x, 16.0, position.z, 1.0};
    int k = 1;
    for (auto &spot : Spots) {
        pos.spots[k] = vec4(spot.first, 1.0);
        lights.spots[k] = {0.01f * spot.second,
                           0.3f * spot.second,
                           spot.second,
                           0.00, 0.005};
        k++;
    }
    lights.spotCount = k;
    LightSetup = lights;
    LightsPos = pos;
    glClearColor(.05, .01, .07, 1);
    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void TScene::DrawScene(IShaderSet &&set) {
    DrawFountain(set);
    DrawObjects(set);
    DrawOpaques(set);
}

void TScene::DrawSkybox() {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    auto setup = TSkyBoxSetup(&SkyboxShader).SetSkyBox(SkyTex);
    Sky.Draw();
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void TScene::UpdateFountain(float interval) {
    double meter = 15 / 1.8;
    double gravity = 9.8 * meter;

    for (int i = 0; i < 30 && CurrentParticles < Particles.size(); ++i) {
        Particles[CurrentParticles++] = Injector.Inject();
    }
    for (int i = 0; i < CurrentParticles; ++i) {
        auto[pos, speed] = Particles[i];
        speed = vec3(speed.x, speed.y - gravity * interval, speed.z);
        if (speed.y < -2 * meter) {
            Particles[i] = Injector.Inject();
        } else {
            Particles[i] = make_tuple(pos + speed * interval, speed);
        }
    }
    Points.GetInstances().Write(Particles.data(), 0, Particles.size() * 6 * sizeof(float));
}

void TScene::DrawFountain(IShaderSet &set) {
    set.Particles(NConstMath::Translate(0, 10, 0), NConstMath::Scale(.5), Points);
}

mat4 Place(vec3 position, vec3 axis, float angle, vec3 s) {
    return NConstMath::Translate(position) * NConstMath::RotateAxis(angle, axis) * NConstMath::Scale(s);
}

void TScene::DrawObjects(IShaderSet &set) {
    set.Scene(scale(NConstMath::Translate(0.0f, -100.0f, 0.0f), vec3(200.0f)),
              false, 0, Asphalt, GroundCube);

    set.Scene(Place(vec3(6, 7.0, 44.0), vec3(.2, .4, -.1), 30.0f, vec3(10.0f)),
              false, 0, Container, SimpleCube);
    float explosion = ExplosionTime <= 14 ? 0.0f : static_cast<float>(std::sin((ExplosionTime - 14) * M_PI) * 20.0f);
    set.Scene(NConstMath::Translate(0, 0, -15), false, explosion, Suit);
}

void TScene::DrawOpaques(IShaderSet &set) {
    std::map<float, OpaqueType> objs{};
    for (auto obj : OpaqueObjects) {
        objs.emplace(-glm::length(get<0>(obj) - set.GetPosition()), obj);
    }
    for (auto obj : objs) {
        // todo: don't divide to position and matrix
        auto&[position, matrix, material, mesh] = obj.second;
        set.Scene(NConstMath::Translate(position) * matrix, true, 0, material, mesh);
    }
}

void TScene::DrawBorder() {
    {
        auto setup = TSilhouetteSetup(&SilhouetteShader).SetModel(NConstMath::Translate(0, 0, -15));
        TFrameBufferBinder binder(AliasedFrameBuffer);
        Suit.Draw(setup);
    }
    AliasedFrameBuffer.CopyTo(FrameBuffer);
    {
        auto setup = TBorderSetup(&BorderShader)
            .SetColor(vec4(0, 1, .5, .3))
            .SetKernel(2.4, 1.2)
            .SetScreen(std::get<TFlatTexture>(FrameBuffer.GetScreen()))
            .SetDepth(std::get<TFlatTexture>(FrameBuffer.GetDepth()));
        ScreenQuad.Draw();
    }
}

void TScene::DrawLightCubes() {
    TLightSetup setup(&LightShader);
    for (auto &spot : Spots) {
        setup.SetModel(scale(translate(one<mat4>(), spot.first), vec3(.5f))).SetColor(spot.second);
        SimpleCube.Draw();
    }
}

