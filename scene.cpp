#include "errors.h"
#include "scene.h"
#include "framebuffer.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

void TScene::Draw(mat4 project, mat4 view, vec3 position, float interval) {
    ExplosionTime = ExplosionTime >= 15 ? 0 : ExplosionTime + interval;
    SetupLights(position);
    UpdateFountain(interval);

    TFrameBufferBinder binder(GlobalLightShadow);
    mat4 lightMatrix = ortho(-70.0f, 70.0f, -70.0f, 70.0f, 0.01f, 150.0f) *
        lookAt(-normalize(Directional) * 60.0f, vec3(0, 0, 0), vec3(0, 1, 0));
    glCullFace(GL_FRONT);
    DrawScene(TShadowShaderSet(&ShadowShader, lightMatrix, position));
    glCullFace(GL_BACK);
    binder.Unbind();

    //auto setup = TDepthSetup(&DepthShader).SetDepth(GlobalLightShadow.GetDepthTexture()).SetPerspective(0, 0);
    //ScreenQuad.Draw();

    ProjectionView = {project, view};
    DrawSkybox();
    DrawScene(TSceneShaderSet{&SceneShader, &ParticlesShader, SkyTex,
                              GlobalLightShadow.GetDepthTexture(), lightMatrix, position});
    DrawLightCubes();
    DrawBorder();
}

void TScene::SetupLights(glm::vec3 position) {
    TLights setup{};
    setup.directional = {Directional, vec3(.25), vec3(.7), vec3(.8)};
    setup.spots[0] = {{position.x, 16.0, position.z},
                      vec3(0.01f), vec3(0.3f), vec3(0.0f),
                      0.00, 0.03};
    int k = 1;
    for (auto &spot : Spots) {
        setup.spots[k] = {spot.first,
                          0.01f * spot.second,
                          0.3f * spot.second,
                          spot.second,
                          0.00, 0.03};
        k++;
    }
    setup.spotCount = k;
    LightSetup = setup;
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
    TArrayBufferMapper<vec3> mapper(Points.GetInstances());
    auto p = *mapper;
    for (int i = 0; i < CurrentParticles; ++i) {
        auto[pos, speed] = Particles[i];
        speed = vec3(speed.x, speed.y - gravity * interval, speed.z);
        if (speed.y < -2 * meter) {
            Particles[i] = Injector.Inject();
        } else {
            Particles[i] = make_tuple(pos + speed * interval, speed);
        }
        *p++ = pos;
        *p++ = speed;
    }
    mapper.Unmap();
}

void TScene::DrawFountain(IShaderSet &set) {
    set.Particles(NConstMath::Translate(0, 10, 0), NConstMath::Scale(.5), Points);
}

mat4 Place(vec3 position, vec3 axis, float angle, vec3 s) {
    return NConstMath::Translate(position) * NConstMath::RotateAxis(angle, axis) * NConstMath::Scale(s);
}

void TScene::DrawObjects(IShaderSet &set) {
    set.Scene(scale(translate(one<mat4>(), vec3(0.0f, -100.0f, 0.0f)), vec3(200.0f)),
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
            .SetScreen(FrameBuffer.GetScreenTexture())
            .SetDepth(FrameBuffer.GetDepthTexture());
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

